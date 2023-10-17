#pragma once

#include <types.h>

namespace zenith::gs {
    // After some years of development, the Z mapping tables have been removed,
    // with the commit message stating, "They aren't just an offset of the base value
    // like the color formats, but instead an XOR of the associated color format"

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

