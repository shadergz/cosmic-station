#pragma once

#include <optional>
#include <os/neon_simd.h>
namespace cosmic::gs {
    class GSEngine;

    class GifArk {
    public:
        GifArk() = default;
        bool downloadGsData(os::machVec128& put);
    private:
        raw_reference<GSEngine> gs;
    };
}