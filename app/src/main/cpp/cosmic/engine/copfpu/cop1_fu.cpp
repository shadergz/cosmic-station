#include <engine/copfpu/cop1_fu.h>

namespace cosmic::engine::copfpu {
    CoProcessor1::CoProcessor1() {
    }

    void CoProcessor1::resetFlu() {
        f64* fuRegs{bit_cast<f64*>(FPRs.data())};
        f512 zero{};

        for (u8 maxRange{}; maxRange < (sizeof(FPRs) / sizeof(f512)); maxRange++)
            vst1q_f64_x4(fuRegs, zero);
    }
}