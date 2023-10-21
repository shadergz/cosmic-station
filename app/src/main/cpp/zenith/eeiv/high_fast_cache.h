#pragma once

#include <array>

#include <types.h>
namespace zenith::eeiv {
struct alignas(8) EECacheLine {
   std::array<u32, 2> tags;
   u32 data[2];
   bool lrf[2];
};
}

