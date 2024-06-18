#pragma once

#include <common/types.h>
namespace cosmic::os {
    struct vec {
        vec(u64 qWord0, u64 qWord1 = 0) {
            native = vsetq_lane_u64(qWord0, native, 0);
            native = vsetq_lane_u64(qWord1, native, 1);
        }
        vec(const u128 val) {
            native = val;
        }
        vec() {
            auto mask{static_cast<u128>(vmovq_n_u64(0))};
            // The mask will be combined with the first value passed to vsetq_lane_u64 to form
            // the value to be stored
            mask = vsetq_lane_u64(0, mask, 0);
            mask = vsetq_lane_u64(0, mask, 1);

            native = vandq_u64(native, mask);
        }
        auto get() const {
            return native;
        }
        inline u32 to32(u8 lane) {
            auto order64{to64(lane >= 2 ? 0 : 1)};
            if (lane == 2) lane = 0;
            if (lane == 3) lane = 1;
            return lane == 0 ?
                order64 >> 32 : static_cast<u32>(order64);
        }
        inline u64 to64(u8 lane) {
            auto order{lane == 0 ? vget_high_u64(native) : vget_low_u64(native)};
            return vget_lane_u64(order, 0);
        }
        template <typename T, u64 lane = 0>
        T as() {
            if constexpr (sizeof(T) == 4) {
                return static_cast<T>(to32(lane));
            } else if constexpr (sizeof(T) == 8) {
                return static_cast<T>(to64(lane));
            }
            return {};
        }
        u64& operator[](u32 vec) {
            return reinterpret_cast<u64*>(&native)[vec];
        }
        u64 operator[](u32 vec) const {
            return reinterpret_cast<const u64*>(&native)[vec];
        }
        os::vec operator&(os::vec& vv) {
            return {
                vv.to64(0) & to64(0),
                vv.to64(1) & to64(1)};
        }

        void operator=(const vec& super) {
            native = super.native;
        }
    private:
        u128 native;
    };
}
