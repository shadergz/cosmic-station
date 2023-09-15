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

        // The first register of the EE is register zero, and its value is always zero. Any attempt
        // to write to it is discarded by default
        m_GPRs[0].qw = 0;

        // Signals to the BIOS that the EE is in its reset process, so it will start our registers
        m_copCPU0.pRid = 0x2e20;
    }
}
