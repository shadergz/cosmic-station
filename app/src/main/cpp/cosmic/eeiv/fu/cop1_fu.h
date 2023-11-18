#pragma once

#include <common/types.h>
namespace cosmic::eeiv::fu {
    class CoProcessor1 {
    public:
        CoProcessor1();
        void resetFlu();

        union {
            std::array<f32, 32> FPRs;
        };
    };
}
