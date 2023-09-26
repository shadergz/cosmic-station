#pragma once
#include <ImplTypes.h>

namespace zenith::iop {
    class IOMipsCore {
    public:
        IOMipsCore();
        void resetIOP();
    public:
        u32 ioPc{};
    };
}

