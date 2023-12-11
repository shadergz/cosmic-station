#pragma once

#include <common/types.h>
namespace cosmic::ipu {
    class [[maybe_unused]] IpuMpeg2 {
    public:
        IpuMpeg2();
        void resetDecoder();

        static std::array<u32, 0x40> inverseScanZZ;
        static std::array<u32, 0x40> inverseScanAlternate;
        // A device or algorithmic function that performs quantization is called a quantizer.
        // An analog-to-digital converter is an example of a quantizer
        static std::array<u32, 0x20> quantizerLinear;
        static std::array<u32, 0x20> quantizerNonLinear;
    };

}
