#include <arm_neon.h>
#include <eeiv/cop0.h>

namespace zenith::eeiv::cop {
    CoProcessor0::CoProcessor0() {
        std::memset(&cops, 0, sizeof(cops));

        // Enabling level 2 exception handling and forcing level 1 exception handling, then
        // Jumping to the bootstrap vectors (bev == 1 && errorLevel == 1)
        // cops.status.rawStatus = 0x400004;
        cops.status.copUsable = 0x7;

        for (u8 regs{}; regs != cop0RegsCount; regs += 4) {
            uint32x2x4_t zero{};
            vst1_u32_x4(m_copGPRs + regs, zero);
        }
        // Co-processor revision ID
        m_copGPRs[15] = 0x00002e20;
    }

}