#pragma once

#include <bitset>
#include <string>
#include <array>

#include <common/types.h>
namespace cosmic::cpu {
    struct FeaturesImpl {
        bool isArchOptional;
        std::string family;
    };
    struct ISA64 {
        enum {
            Crc32
        };
        static constexpr auto size{Crc32 + 1};
    };
    extern const std::array<FeaturesImpl, ISA64::size> features;

    class HostFeatures {
    public:
        HostFeatures();
        bool haveCrc32C();
    private:
        std::bitset<std::size(features)> mrsA64;
    };
}

