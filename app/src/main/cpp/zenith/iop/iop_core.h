#pragma once
#include <common/types.h>

namespace zenith::iop {
    class IOMipsCore {
    public:
        IOMipsCore();
        void resetIOP();

        void pulse(u32 cycles);
    public:
        u32 ioPc{},
            cyclesToIO;
    };
}

