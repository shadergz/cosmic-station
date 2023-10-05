#pragma once

#include <arm_neon.h>
#include <types.h>
namespace zenith::os {
    using uNative128 = uint64x2_t;

    struct machVec128 {
        machVec128(u64 qWord0 = 0, u64 qWord1 = 0) {
            vec128 = vsetq_lane_u64(qWord0, vec128, 0);
            vec128 = vsetq_lane_u64(qWord1, vec128, 1);
        }

        machVec128() {
            auto mask{static_cast<uNative128>(vmovq_n_u64(0xffffffffffffffffull))};
            // The mask will be combined with the first value passed to vsetq_lane_u64 to form
            // the value to be stored
            mask = vsetq_lane_u64(0, mask, 0);
            mask = vsetq_lane_u64(0, mask, 1);

            vec128 = vandq_u64(vec128, mask);
        }
        void operator=(const machVec128& super) {
            vec128 = super.vec128;
        }
    private:
        uNative128 vec128;
    };
}
