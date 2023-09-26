#include <arm_neon.h>
#include <eeiv/Cop0.h>

namespace zenith::eeiv {
    CoProcessor0::CoProcessor0() {
        status.copUsable = 0x7;

        for (u8 regs{}; regs != cop0RegsCount; regs += 4) {
            uint32x2x4_t zero{};
            vst1_u32_x4(copGPRs + regs, zero);
        }
        // Signals to the BIOS that the EE is in its reset process, so it will start our registers
        // Co-processor revision ID
        pRid = 0x59;
    }

}