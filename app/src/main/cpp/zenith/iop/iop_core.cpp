#include <iop/iop_core.h>

namespace zenith::iop {
    IOMipsCore::IOMipsCore() {}

    void IOMipsCore::resetIOP() {
        // The IOP processor initializes the PC at the same address as the EE
        ioPc = 0xbfc00000;
    }

    void IOMipsCore::pulse(u32 cycles) {
        cyclesToIO += cycles;
    }
}
