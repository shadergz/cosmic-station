#include <iop/iop_intc.h>
namespace zenith::iop {
    void IopINTC::iopCheck() {
        iop->intByINTC(ctrl && (stat & mask));
    }
}