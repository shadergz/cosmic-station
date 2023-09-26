#include <iop/IOPCore.h>

namespace zenith::iop {
    IOMipsCore::IOMipsCore() {
        resetIOP();
    }

    void IOMipsCore::resetIOP() {
        // The IOP processor initializes the PC at the same address as the EE
        ioPc = 0xbfc00000;
    }
}
