#pragma once
#include <ImplTypes.h>
namespace zenith::eeiv {
    struct EECacheLine {
        u32 tags[2];
        bool lfu[2];
    };
}

