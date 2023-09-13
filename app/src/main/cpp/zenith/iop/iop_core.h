#pragma once

#include <impl_types.h>

namespace zenith::iop {
    class IOMipsCore {
    public:
        IOMipsCore();
        void resetIOP();
    public:
        u32 m_ioPc{};
    };
}

