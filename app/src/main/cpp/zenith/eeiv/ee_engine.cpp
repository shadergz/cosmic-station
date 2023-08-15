#include <eeiv/ee_engine.h>

namespace eeiv {
    EECoreCpu::EECoreCpu() {
        // Allocating 32 megabytes of RAM to the primary CPU
        // In a simulated hardware environment, we could simply create an array of bytes to serve
        // as our RAM without any issues
        mainRamBlock = new uint8_t[32 * 1024 * 1024];
        gprs = new eeRegister[countOfGPRs];

        resetCore();
    }

    EECoreCpu::~EECoreCpu() {
        delete[] mainRamBlock;
        delete[] gprs;
    }

    void EECoreCpu::resetCore() {
        // The BIOS should be around here somewhere
        regPC = 0xbfc00000;

        gprs[0].qw = 0;

    }
}
