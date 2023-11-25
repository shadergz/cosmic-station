#pragma once

#include <common/types.h>
#include <vu/vecu.h>

namespace cosmic::vu {
    // Just a communication interface between these two VUs
    class [[maybe_unused]] V01Cop2 {
    public:
        raw_reference<vu::VectorUnit> v0;
        raw_reference<vu::VectorUnit> v1;
    };
}

