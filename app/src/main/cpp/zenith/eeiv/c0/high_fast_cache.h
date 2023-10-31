#pragma once

#include <array>
#include <common/types.h>
namespace zenith::eeiv::c0 {
    struct alignas(8) EECacheLine {
       std::array<u32, 2> tags;
       u32 data[2];
       bool lrf[2];
    };
}
