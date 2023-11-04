#include <eeiv/fu/cop1_fu.h>

namespace zenith::eeiv::fu {
    CoProcessor1::CoProcessor1() {
    }

    void CoProcessor1::resetFlu() {
        float32x4_t zero{};
        float32x4_t* fuRegs{reinterpret_cast<float32x4_t*>(FPRs.data())};

        for (u8 maxRange{}; maxRange < 8; maxRange++) {
            *(fuRegs + maxRange) = vmulq_f32(zero, zero);
        }
    }
}