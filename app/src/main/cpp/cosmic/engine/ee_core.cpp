#include <common/global.h>

#include <engine/ee_core.h>
#include <engine/copctrl/cop0.h>

#include <creeper/ee/mipsiv_cached.h>
#include <fishron/ee64/jitter_arm64_ee.h>
#include <console/virt_devices.h>
namespace cosmic::engine {
    EeMipsCore::~EeMipsCore() {
        eePc = 0xffffffff;
        memset(GPRs.data(), 0xff, sizeof(GPRs));
    }
    void EeMipsCore::resetCore() {
        // The BIOS should be around here somewhere
        cop0.resetCoP();
        cop1.resetFlu();

        eePc = 0xbfc00000;
        cop0.redoTlbMapping();

        // Cleaning up all registers, including the $zero register
        auto gprs{reinterpret_cast<u64*>(GPRs.data())};
        for (u8 regRange{}; regRange != countOfGPRs; regRange += 8) {
            static u256 zero{};
            // Writing 256 bits (32 bytes) per write call = 2GPRs

            vst1_u64_x4(gprs + regRange, zero);
            vst1_u64_x4(gprs + regRange + 2, zero);
            vst1_u64_x4(gprs + regRange + 4, zero);
            vst1_u64_x4(gprs + regRange + 6, zero);
        }
        runCycles = cycles[0] = 0;
        user->info("(EE): Emotion Engine is finally reset to default, " \
            "GPR {}: {}", gprsId[15], fmt::join(GPRs[15].dw, ", "));
    }
    void EeMipsCore::pulse(u32 cycles) {
        this->cycles[0] = cycles;
        cop0.count += cycles;
        if (!irqTrigger) {
            i64 beforeInc{runCycles};
            runCycles += cycles;
            if (beforeInc >= 0) {
                executor->executeCode();
#if !defined(NDEBUG)
                printStates();
#endif
            }
        } else {
            runCycles = cycles;
            executor->executeCode();
        }
        cop0.rectifyTimer(cycles);
        if (cop0.isIntEnabled()) {
            if (cop0.cause.timerIp)
                ;
        }
    }
    u32 EeMipsCore::fetchByPc() {
        const u32 orderPC{*lastPc};
        [[unlikely]] if (!cop0.virtCache->isCached(*eePc)) {
            // However, the EE loads two instructions at once
            u32 punishment{8};
            if ((orderPC + 4) != *eePc) {
                // When reading an instruction out of sequential order, a penalty of 32 cycles is applied
                punishment = 32;
            }
            // Loading just one instruction, so, we will divide this penalty by 2
            runCycles -= punishment / 2;
            return mipsRead<u32>(incPc());
        }
        if (!cop0.isCacheHit(*eePc, 0) && !cop0.isCacheHit(*eePc, 1)) {
            cop0.loadCacheLine(*eePc, *this);
        }
        auto pcCached{cop0.readCache(*eePc)};
        return pcCached.to32(incPc() & 3);
    }
    u32 EeMipsCore::fetchByAddress(u32 address) {
        lastPc = address;
        [[unlikely]] if (!cop0.virtCache->isCached(address)) {
            runCycles -= 8 / 2;

            return mipsRead<u32>(address);
        } else if (!cop0.isCacheHit(address, 0) && !cop0.isCacheHit(address, 1)) {
            cop0.loadCacheLine(address, *this);
        }
        return cop0.readCache(address).to32(address & 3);
    }
    EeMipsCore::EeMipsCore(std::shared_ptr<mio::MemoryPipe>& pipe) :
        cop0(pipe->controller),
        observer(pipe) {

        GPRs[0].dw[0] = 0;
        GPRs[0].dw[1] = 0;

        device->getStates()->addObserver(os::EeMode, [this](JNIEnv* os) {
            procCpuMode = static_cast<ExecutionMode>(*device->getStates()->eeMode);
            if (executor)
                executor.reset();
            if (procCpuMode == CachedInterpreter) {
                executor = std::make_unique<creeper::ee::MipsIvInterpreter>(*this);
            } else if (procCpuMode == JitRe) {
                executor = std::make_unique<fishron::ee64::EeArm64Jitter>(*this);
            }
        });
    }
    void EeMipsCore::invalidateExecRegion(u32 address) {
        if (address & 0x1fffffff)
            return;
        if (executor)
            executor->performInvalidation(address);
    }
    void EeMipsCore::branchByCondition(bool cond, i32 jumpRel) {
        if (!cond)
            return;
        isABranch = cond;
        i64 pc{static_cast<i64>(*eePc) + jumpRel + 4};
        eePc = static_cast<u32>(pc);
        delaySlot = 1;
    }
    void EeMipsCore::branchOnLikely(bool cond, i32 jumpRel) {
        if (cond)
            branchByCondition(true, jumpRel);
        else
            incPc();
    }
    void EeMipsCore::updateTlb() {
        cop0.redoTlbMapping();
    }
    void EeMipsCore::setTlbByIndex() {
        auto selectedLb{std::ref(cop0.virtCache->entries[cop0.tlbIndex])};

        cop0.virtCache->unmapTlb(selectedLb);
        cop0.configureGlobalTlb(selectedLb);
        cop0.virtCache->mapTlb(selectedLb);
    }
    RawReference<mio::TlbPageEntry> EeMipsCore::fetchTlbFromCop(u32* c0Regs) {
        u16 c0id{*reinterpret_cast<u16*>(c0Regs[0])};
        return cop0.virtCache->entries[c0id];
    }
    void EeMipsCore::handleException(u8 el, u32 exceptVec, u8 code) {
        cop0.cause.exCode = code & 0xd;
        const u8 savePcId{static_cast<u8>(el == 1 ? 14 : 30)};
        if (isABranch) {
            cop0.mtc0(savePcId, *eePc - 4);
        } else {
            cop0.mtc0(savePcId, *eePc);
        }
        if (savePcId == 14) {
            cop0.cause.bd = isABranch;
            cop0.status.exception = true;
        } else {
            cop0.cause.bd2 = isABranch;
            cop0.status.error = true;
        }

        if (cop0.status.bev || cop0.status.dev) {
            exceptVec |= 0xbfc00;
            exceptVec += 200;
        }
        isABranch = false;
        delaySlot = 0;
        chPc(exceptVec);
        cop0.redoTlbMapping();
    }

    void EeMipsCore::printStates() {
        fmt::memory_buffer states;
        fmt::format_to(back_inserter(states), "PC: {:#x}\n", eePc.pcValue);

        for (u32 ir{}; ir < 32; ir++) {
            fmt::format_to(back_inserter(states), "EE-GPR {}: dw0: {:#x}, dw1: {:#x}\n",
                gprsId[ir], GPRs[ir].dw[0], GPRs[ir].dw[1]);
        }
        fmt::format_to(back_inserter(states), "LO: {:#x}\n", mulDivStorage[0]);
        fmt::format_to(back_inserter(states), "HI: {:#x}\n", mulDivStorage[1]);
        static const std::array<std::string, 3> ksu{"Kernel", "Super", "User"};
        fmt::format_to(back_inserter(states), "KSU: {}\n", ksu[cop0.status.mode]);

        for (u32 cg{}; cg < 32; cg++)
            fmt::format_to(back_inserter(states), "EE-COP0: ID: {:#x}, Value {:#x}\n", cg, cop0.GPRs[cg]);
        for (u32 fg{}; fg < 32; fg++)
            fmt::format_to(back_inserter(states), "EE-COP1: ID: {:#x}, Value {:f}\n", fg, cop1.fprRegs[fg].decimal);

        user->info("{}", states.data());
    }
    i64 EeMipsCore::getHtzCycles(bool total) const {
        return total ? cop0.count : runCycles;
    }
}
