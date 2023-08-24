#pragma once

#include <arm_neon.h>

namespace os {
    struct native128 {
        native128(uint64_t qWord0 = 0, uint64_t qWord1 = 0) {
            vec128 = vsetq_lane_u64(qWord0, vec128, 0);
            vec128 = vsetq_lane_u64(qWord1, vec128, 1);
        }

        native128() {
            uint64x2_t mask{vmovq_n_u64(0xFFFFFFFFFFFFFFFFull)};
            // The mask will be combined with the first value passed to vsetq_lane_u64 to form
            // the value to be stored
            mask = vsetq_lane_u64(0, mask, 0);
            mask = vsetq_lane_u64(0, mask, 1);

            vec128 = vandq_u64(vec128, mask);
        }
        void operator=(const native128& super) {
            vec128 = super.vec128;
        }
    private:
        uint64x2_t vec128;
    };
}
