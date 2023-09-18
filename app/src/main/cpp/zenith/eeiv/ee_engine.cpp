#include <eeiv/ee_engine.h>
#include <eeiv/cop0.h>

namespace zenith::eeiv {
    EEMipsCore::EEMipsCore(const std::shared_ptr<console::GlobalMemory>& memoryChips)
        : m_sharedMemory(memoryChips) {
        // Allocating 32 megabytes of RAM to the primary CPU
        // In a simulated hardware environment, we could simply create an array of bytes to serve
        // as our RAM without any issues
        m_GPRs = new eeRegister[countOfGPRs];

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
            m_hiCache[line].tags[0] = invLane01Cache;
            m_hiCache[line].tags[1] = invLane01Cache;

            m_hiCache[line].lfu[0] = false;
            m_hiCache[line].lfu[1] = false;
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
