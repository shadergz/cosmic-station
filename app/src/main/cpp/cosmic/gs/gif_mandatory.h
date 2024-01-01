#pragma once

#include <optional>
#include <os/neon_simd.h>
namespace cosmic::gs {
    class GsEngine;

    class GifArk {
    public:
        GifArk() = default;
        bool downloadGsData(os::vec& put);
    private:
        RawReference<GsEngine> gs;
    };
}