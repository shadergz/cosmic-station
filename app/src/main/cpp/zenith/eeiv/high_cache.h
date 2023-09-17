#pragma once
#include <impl_types.h>
namespace zenith::eeiv {
    struct EECacheLine {
        u32 tags[2];
        bool lfu[2];
    };
}

