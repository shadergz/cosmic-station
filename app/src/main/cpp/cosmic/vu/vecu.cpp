#include <vu/vecu.h>

namespace cosmic::vu {
    void VectorUnit::pulse(u32 cycles) {
    }
    void VectorUnit::resetVU() {
        // Performs a software-level reset
        softwareReset();
        vuPC = 0;
    }
    // The VUs allow for software reset; this reset does not affect the VU in such
    // a way that it returns to the Genesis state
    void VectorUnit::softwareReset() {
        // DS|IS|OS|US|SS|ZS|D |I |O |U |S |Z = 0
        status = 0;
        isVUBusy = isOnBranch = false;
        pipeStates[0] = 0;
        pipeStates[1] = 0;

        // Cleaning our states (these states are used to control the internal VU system)
        for (u8 clean{}; clean < 4; clean++) {
            clipFlags[clean] = 0;
            macFlags[clean] = 0;
        }
    }
}