#include <common/global.h>

#include <engine/ee_core.h>
#include <engine/copctrl/cop0.h>

#include <creeper/ee/mipsiv_cached.h>
#include <fishron/ee64/jitter_arm64_ee.h>
#include <console/virt_devices.h>
namespace cosmic::engine {
    EeMipsCore::~EeMipsCore() {
        delete[] GPRs;
    }
    void EeMipsCore::resetCore() {
        // The BIOS should be around here somewhere
        eePc = 0xbfc00000;
        tlbMap = ctrl0.mapVirtualTlb(eeTlb);

        // Cleaning up all registers, including the $zero register
        auto gprs{reinterpret_cast<u64*>(GPRs)};
        for (u8 regRange{}; regRange != countOfGPRs; regRange += 8) {
            static u256 zero{};
            // Writing 256 bits (32 bytes) per write call = 2GPRs

            vst1_u64_x4(gprs + regRange, zero);
            vst1_u64_x4(gprs + regRange + 2, zero);
            vst1_u64_x4(gprs + regRange + 4, zero);
            vst1_u64_x4(gprs + regRange + 6, zero);
        }
        runCycles = cycles[0] = 0;
        userLog->info("(EE): Emotion Engine is finally reset to default, " \
            "GPR {}: {}", gprsId[15], fmt::join(GPRs[15].dw, ", "));
    }
    void EeMipsCore::pulse(u32 cycles) {
        this->cycles[0] = cycles;
        ctrl0.count += cycles;
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
        ctrl0.rectifyTimer(cycles);
        if (ctrl0.isIntEnabled()) {
            if (ctrl0.cause.timerIP)
                ;
        }
    }
    u32 EeMipsCore::fetchByPc() {
        const u32 orderPC{*lastPc};
        [[unlikely]] if (!eeTlb->isCached(*eePc)) {
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
        if (!ctrl0.isCacheHit(*eePc, 0) && !ctrl0.isCacheHit(*eePc, 1)) {
            ctrl0.loadCacheLine(*eePc, *this);
        }
        return ctrl0.readCache(incPc());
    }
    u32 EeMipsCore::fetchByAddress(u32 address) {
        lastPc = address;
        [[unlikely]] if (!eeTlb->isCached(address)) {
            runCycles -= 8 / 2;

            return mipsRead<u32>(address);
        } else if (!ctrl0.isCacheHit(address, 0) && !ctrl0.isCacheHit(address, 1)) {
            ctrl0.loadCacheLine(address, *this);
        }
        return ctrl0.readCache(address);
    }
    EeMipsCore::EeMipsCore(std::shared_ptr<mio::MemoryPipe>& pipe) :
        ctrl0(pipe->controller),
        observer(pipe) {

        GPRs = new eeRegister[countOfGPRs];
        GPRs[0].dw[0] = 0;
        GPRs[0].dw[1] = 0;
        eeTlb = std::make_shared<mio::TlbCache>(observer->controller->mapped);

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
}
