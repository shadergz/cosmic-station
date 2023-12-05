#pragma once

#include <common/types.h>
#include <vu/vecu.h>

namespace cosmic::vu {
    // Just a communication interface between these two VUs
    class MacroModeCop2 {
    public:
        MacroModeCop2(raw_reference<vu::VectorUnit> vus[2]);
        u32 cfc2(u32 special);
        void ctc2(u32 special, u32 value);

        raw_reference<vu::VectorUnit> v0;
        raw_reference<vu::VectorUnit> v1;
    };
}

