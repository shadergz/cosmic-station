#pragma once

#include <arm_neon.h>

namespace os {
    struct native128 {
        native128(uint64_t qWord0 = 0, uint64_t qWord1 = 0) {
            vecDQWord = vsetq_lane_u64(qWord0, vecDQWord, 0);
            vecDQWord = vsetq_lane_u64(qWord1, vecDQWord, 1);
        }

        native128() {
            uint64x2_t mask{vmovq_n_u64(0xFFFFFFFFFFFFFFFFULL)};
            // The mask will be combined with the first value passed to vsetq_lane_u64 to form
            // the value to be stored
            mask = vsetq_lane_u64(0, mask, 0);
            mask = vsetq_lane_u64(0, mask, 1);

            vecDQWord = vandq_u64(vecDQWord, mask);
        }
        void operator=(const native128& super) {
            vecDQWord = super.vecDQWord;
        }
    private:
        uint64x2_t vecDQWord;
    };
}
