#pragma once

#include <types.h>

namespace zenith::gs {
    extern const u8 blockTable32[4][8];
    extern const u8 blockTable16[8][4];
    extern const u8 blockTable16S[8][4];
    extern const u8 blockTable8[4][8];
    extern const u8 blockTable4[8][4];

    extern const u8 columnTable32[8][8];
    extern const u8 columnTable16[8][16];
    extern const u8 columnTable8[16][16];
    extern const u16 columnTable4[16][32];

    extern const u8 clutTableT32I8[128];
    extern const u8 clutTableT32I4[16];
    extern const u8 clutTableT16I8[32];
    extern const u8 clutTableT16I4[16];
}

