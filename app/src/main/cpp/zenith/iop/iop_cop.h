#pragma once
#include <common/types.h>
namespace zenith::iop {
    struct IopCopStatus {
        bool bev;
    };
    struct IopCopCause {
        u8 intPending;
    };

    class IopCop {
    public:
        IopCopStatus status;
        IopCopCause cause;
        u32 ePC;

        void resetIOCop();
    };
}