#include <iop/iop_cop.h>

namespace zenith::iop {
    void IopCop::resetIOCop() {
        status.bev = true;
        cause.intPending = 0;
    }
}
