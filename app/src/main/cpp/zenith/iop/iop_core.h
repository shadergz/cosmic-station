#pragma once

#include <impl_types.h>

namespace iop {
    class IOMipsCore {
    public:
        IOMipsCore();

        void resetIOP();

    public:
        uint32_t m_ioPc{};
    };
}

