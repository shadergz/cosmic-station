#pragma once

#include <common/types.h>
namespace cosmic::os {
    struct vec {
        vec() = default;
        vec(u64 qWord0, u64 qWord1 = 0) {
            native = vsetq_lane_u64(qWord0, native, 0);
            native = vsetq_lane_u64(qWord1, native, 1);
        }
        vec(const u128 val) {
            native = val;
        }
        auto get() const {
            return native;
        }
        inline u32 to32(const u64 lane) const {
            switch (lane) {
            case 0: return vgetq_lane_u32(native, 0);
            case 1: return vgetq_lane_u32(native, 1);
            case 2: return vgetq_lane_u32(native, 2);
            case 3: return vgetq_lane_u32(native, 3);
            }
            return {};
        }
        inline u64 to64(const u64 lane) const {
            return lane == 0 ? vgetq_lane_u64(native, 0) : vgetq_lane_u64(native, 1);
        }
        template <typename T, u64 lane = 0>
        T as() const {
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
        os::vec operator&(const os::vec& vv) const {
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
