#include <common/global.h>

#include <ee/ee_core.h>
#include <ee/cop0.h>

#include <creeper/cached_blocks.h>
#include <fishron/jitter_arm64_ee.h>
#include <console/virt_devices.h>
namespace cosmic::ee {
    EeMipsCore::~EeMipsCore() {
        eePc = 0xffffffff;
        memset(GPRs.data(), 0xff, sizeof(GPRs));

    }
    void EeMipsCore::resetCore() {
        // The BIOS should be around here somewhere
        cop0.resetCoP();
        cop1.resetFlu();
        timer->resetTimers();

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
        runCycles = {};
        user->info("(EE): Emotion Engine is finally reset to default, "
            "GPR {}: {}", eeAllGprIdentifier[15], fmt::join(GPRs[15].dw, ", "));
    }
    void EeMipsCore::pulse(u32 cycles) {
        cop0.count += cycles;
        if (!irqTrigger) {
            const auto beforeInc{runCycles};
            runCycles += cycles;
            if (beforeInc >= 0) {
                executor->executeCode();
#if !defined(NDEBUG)
                printStates();
#endif
            }
        } else {
            runCycles = static_cast<i32>(cycles);
        }
        cop0.rectifyTimer(cycles);
        if (cop0.isIntEnabled()) {
            if (cop0.cause.timerIp) {
            }
        }
    }

    u32 EeMipsCore::fetchByPc() {
        const u32 orderPc{lastPc};
        [[unlikely]] if (!cop0.virtCache->isCached(eePc)) {
            // However, the EE loads two instructions at once
            u32 punishment{8};
            if ((orderPc + 4) != eePc) {
                // When reading an instruction out of sequential order, a penalty of 32 cycles is applied
                punishment = 32;
            }
            // Loading just one instruction, so, we will divide this penalty by 2
            runCycles -= punishment / 2;
            return mipsRead<u32>(incPc());
        }
        if (!cop0.isCacheHit(eePc, 0) &&
            !cop0.isCacheHit(eePc, 1)) {
            cop0.loadCacheLine(eePc, *this);
        }
        auto& pcCached{cop0.readCache(eePc)};
        const u32 part{(incPc() & 0xf) / 4};
        return pcCached.to32(part);
    }
    u32 EeMipsCore::fetchByAddress(u32 address) {
        lastPc = address;
        struct CachedAddress{
            const u32 stripPcAddr(u32 addr) const {
                return addr & 0xffff'fff0;
            }
            auto& operator[](const u32 address) {
                return nested[(address & 0xf) / 4];
            }
            u32 basePc;
            bool isValid{};
            std::array<u32, 4> nested;
        };
        static CachedAddress cached;

        if (cop0.virtCache->isCached(address)) {
            if (!cop0.isCacheHit(address, 2)) {
                cop0.loadCacheLine(address, *this);
                cached.isValid = false;
            }
        } else {
            runCycles -= 8 / 2;
            return mipsRead<u32>(address);
        }
        u32 currBase{cached.stripPcAddr(address)};
        if (cached.basePc != currBase)
            cached.isValid = false;

        if (!cached.isValid) {
            const auto& fasterInstructions{cop0.readCache(address)};
            cached[0] = fasterInstructions.to32(0);
            cached[4] = fasterInstructions.to32(1);
            cached[8] = fasterInstructions.to32(2);
            cached[12] = fasterInstructions.to32(3);
            cached.basePc = currBase;
            cached.isValid = true;
        }
        return cached[address];
    }
    EeMipsCore::EeMipsCore(std::shared_ptr<mio::MemoryPipe>& pipe) :
        cop0(pipe->controller),
        memPipe(pipe) {

        GPRs[0].dw[0] = 0;
        GPRs[0].dw[1] = 0;

        states->eeMode.addListener([&]() {
            cpuMode = static_cast<ExecutionMode>(*states->eeMode);
            if (executor)
                executor.reset();
            if (cpuMode == CachedInterpreter) {
                executor = std::make_unique<creeper::MipsIvInterpreter>(Ref(*this));
            } else if (cpuMode == JitRe) {
                executor = std::make_unique<fishron::EeArm64Jitter>(Ref(*this));
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
        const auto pc{static_cast<i64>(eePc) + jumpRel + 4};
        eePc = static_cast<u32>(pc);
        delaySlot = 1;
    }
    void EeMipsCore::branchOnLikely(bool cond, i32 jumpRel) {
        if (cond)
            branchByCondition(true, jumpRel);
        else
            incPc();
    }
    void EeMipsCore::setTlbByIndex() {
        auto& selected{cop0.virtCache->entries[cop0.tlbIndex]};
        cop0.virtCache->unmapTlb(selected);

        cop0.configureGlobalTlb(selected);
        cop0.virtCache->mapTlb(selected);
    }
    mio::TlbPageEntry& EeMipsCore::fetchTlbFromCop(u32 c0Regs[]) {
        auto c0id{static_cast<u16>(c0Regs[0])};
        return cop0.virtCache->entries[c0id];
    }

    void EeMipsCore::handleException(u8 el, u32 exceptVec, u8 code) {
        cop0.cause.exCode = code & 0xd;
        const auto savePcId{static_cast<u8>(el == 1 ? 14 : 30)};
        if (isABranch) {
            cop0.mtc0(savePcId, eePc - 4);
        } else {
            cop0.mtc0(savePcId, eePc);
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
        unHaltCpu();
        cop0.redoTlbMapping();
    }

    void EeMipsCore::printStates() {
        fmt::memory_buffer states;
        fmt::format_to(back_inserter(states), "PC: {:#x}\n", eePc.pcValue);

        for (u32 ir{}; ir < 32; ir++) {
            fmt::format_to(back_inserter(states), "EE-GPR {}: dw0: {:#x}, dw1: {:#x}\n",
                eeAllGprIdentifier[ir], GPRs[ir].dw[0], GPRs[ir].dw[1]);
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
