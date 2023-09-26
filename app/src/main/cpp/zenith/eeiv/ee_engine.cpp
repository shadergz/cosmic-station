#include <app.h>
#include <eeiv/ee_engine.h>
#include <eeiv/cop0.h>

#include <eeiv/casper/casper_interpreter.h>
#include <eeiv/tokyo3/tokyo3_arm64_jitter.h>

namespace zenith::eeiv {
    EEMipsCore::EEMipsCore(const std::shared_ptr<console::GlobalMemory>& glbRef)
        : m_glbRDRAM(glbRef),
          m_eeTLB(std::make_unique<TLBCache>(glbRef))
          {

        m_GPRs = new eeRegister[countOfGPRs];
        m_eeNearCache = new EECacheLine[countOfCacheLines];

        auto globalStates{deviceRes->getServiceState()};
        m_proCPUMode = static_cast<EEExecutionMode>(*globalStates.lock()->m_cpuExecutor);

        if (m_proCPUMode == EEExecutionMode::CachedInterpreter)
            m_eeExecutor = std::make_unique<casper::EEInterpreter>(*this);
        else if (m_proCPUMode == EEExecutionMode::JitRe)
            m_eeExecutor = std::make_unique<tokyo3::EEArm64Jitter>(*this);

        resetCore();
    }

    EEMipsCore::~EEMipsCore() {
        delete[] m_GPRs;
    }

    void EEMipsCore::resetCore() {
        // The BIOS should be around here somewhere
        m_eePC = 0xbfc00000;

        constexpr auto invLane01Cache{static_cast<u32>(1<<31)};
        // Invalidating all cache lines
        for (u8 line{}; line < countOfCacheLines; line++) {
            m_eeNearCache[line].tags[0] = invLane01Cache;
            m_eeNearCache[line].tags[1] = invLane01Cache;

            m_eeNearCache[line].lfu[0] = false;
            m_eeNearCache[line].lfu[1] = false;
        }

        // Cleaning up all registers, including the $zero register
        for (u8 regRange{}; regRange != countOfGPRs; regRange += 8) {
            static auto gprs{reinterpret_cast<u64*>(m_GPRs)};
            static uint64x1x4_t zero{};
            // Writing 256 bits (32 bytes) per write call

            vst1_u64_x4(gprs + regRange, zero);
            vst1_u64_x4(gprs + regRange + 2, zero);
            vst1_u64_x4(gprs + regRange + 4, zero);
            vst1_u64_x4(gprs + regRange + 6, zero);
        }

    }
}
