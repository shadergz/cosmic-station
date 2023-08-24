#include <iop/iop_core.h>

namespace iop {
    IOMipsCore::IOMipsCore() {
        resetIOP();
    }

    void IOMipsCore::resetIOP() {
        // The IOP processor initializes the PC at the same address as the EE
        m_ioPc = 0xbfc00000;
    }
}
