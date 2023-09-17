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

        for (u8 regRange{}; regRange != countOfGPRs; regRange += 8) {
            // Writing 256 bits (32 bytes) per write call
            static auto zeGPRx2ByLane = [](eeRegister* regIndex, const u8 posIn) {
                static const uint64x1x4_t zero{};
                vst1_u64_x4(reinterpret_cast<u64*>(regIndex) + posIn, zero);
            };

            zeGPRx2ByLane(m_GPRs, regRange + 0);
            zeGPRx2ByLane(m_GPRs, regRange + 2);
            zeGPRx2ByLane(m_GPRs, regRange + 4);
            zeGPRx2ByLane(m_GPRs, regRange + 6);
        }

        // The first register of the EE is register zero, and its value is always zero. Any attempt
        // to write to it is discarded by default
        m_GPRs[0].qw = 0;

        // Signals to the BIOS that the EE is in its reset process, so it will start our registers
        m_copCPU0.pRid = 0x2e20;
    }
}
