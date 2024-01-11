#pragma once

#include <common/types.h>
namespace cosmic::iop {
    enum DmaChannels {
        IopSpu2 = 0x8
    };
    class IopDma {
    public:
        IopDma() {
        }
        void resetIoDma();
    };
}
