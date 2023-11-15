#pragma once

#include <arm_neon.h>
#include <unistd.h>
namespace zenith {
    using i8 = std::int8_t;
    using u8 = std::uint8_t;
    using u16 = std::uint16_t;

    using i32 = std::int32_t;
    using i16 = std::int16_t;

    using u32 = std::uint32_t;
    using u88 = uint8x8_t;

    using i64 = std::int64_t;
    using u64 = std::uint64_t;

    using u256 = uint64x1x4_t;

    using f32 = float32_t;
    using f64 = float64_t;
    using f512 = float64x2x4_t;
}