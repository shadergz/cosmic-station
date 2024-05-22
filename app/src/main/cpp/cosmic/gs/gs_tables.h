#pragma once
#include <common/types.h>
namespace cosmic::gs {
    // After some years of development, the Z mapping tables have been removed,
    // with the commit message stating, "They aren't just an offset of the base value
    // like the color formats, but instead an XOR of the associated color format"

    template <size_t S>
    using ConstByteArray = const std::array<u8, S>;
    template <size_t S>
    using ConstWordArray = const std::array<u16, S>;

    extern ConstByteArray<8> blockTable32[4];
    extern ConstByteArray<4> blockTable16[8];
    extern ConstByteArray<4> blockTable16S[8];
    extern ConstByteArray<8> blockTable8[4];
    extern ConstByteArray<4> blockTable4[8];

    extern ConstByteArray<8> columnTable32[8];
    extern ConstByteArray<16> columnTable16[8];
    extern ConstByteArray<16> columnTable8[16];
    extern ConstWordArray<32> columnTable4[16];

    extern ConstByteArray<128> clutTableT32I8;
    extern ConstByteArray<16> clutTableT32I4;
    extern ConstByteArray<32> clutTableT16I8;
    extern ConstByteArray<16> clutTableT16I4;
}

