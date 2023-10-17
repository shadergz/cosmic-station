#include <app.h>
#include <java/device_handler.h>

#include <eeiv/ee_engine.h>
#include <eeiv/cop0.h>

#include <eeiv/fuji/cached_interpreter.h>
#include <eeiv/tokyo3/tokyo3_arm64_jitter.h>

namespace zenith::eeiv {
    EEMipsCore::EEMipsCore(const std::shared_ptr<link::GlobalMemory>& global)
        : glbRDRAM(global),
          eeTLB(std::make_shared<TLBCache>(global)) {

        GPRs = new eeRegister[countOfGPRs];
        auto globalStates{device->getServiceState()};
        proCPUMode = static_cast<EEExecutionMode>(*globalStates.lock()->cpuExecutor);

        if (proCPUMode == EEExecutionMode::CachedInterpreter)
            eeExecutor = std::make_unique<fuji::EEInterpreter>(*this);
        else if (proCPUMode == EEExecutionMode::JitRe)
            eeExecutor = std::make_unique<tokyo3::EEArm64Jitter>(*this);
    }

    EEMipsCore::~EEMipsCore() {
        delete[] GPRs;
    }

    void EEMipsCore::resetCore() {
        // The BIOS should be around here somewhere
        eePC = 0xbfc00000;
        virtTable = cop0.mapVirtualTLB(eeTLB);

        // Cleaning up all registers, including the $zero register
        for (u8 regRange{}; regRange != countOfGPRs; regRange += 8) {
            static auto gprs{reinterpret_cast<u64*>(GPRs)};
            static uint64x1x4_t zero{};
            // Writing 256 bits (32 bytes) per write call

            vst1_u64_x4(gprs + regRange, zero);
            vst1_u64_x4(gprs + regRange + 2, zero);
            vst1_u64_x4(gprs + regRange + 4, zero);
            vst1_u64_x4(gprs + regRange + 6, zero);
        }
    }

    u32 EEMipsCore::fetchByPC() {
        [[unlikely]] if (!eeTLB->isCached(*eePC)) {
            // When reading an instruction out of sequential order, a penalty of 32 cycles is applied.
            // However, the EE loads two instructions at once, but in this case, we are only
            // loading one instruction. So, we will divide this penalty by 2 :0
            cyclesToWaste -= 16;
            lastPC = eePC++;
            return tableRead<u32>(*lastPC);
        }
        if (!cop0.isCacheHit(*eePC, 0) && !cop0.isCacheHit(*eePC, 1)) {
            cop0.loadCacheLine(*eePC, *this);
        }
        lastPC = eePC++;
        return cop0.readCache32(*lastPC);
    }
}
