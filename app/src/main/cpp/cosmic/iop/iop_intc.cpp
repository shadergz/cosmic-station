// SPDX-short-identifier: MIT, Version N/A
// This file is protected by the MIT license (please refer to LICENSE.md before making any changes, copying, or redistributing this software)
#include <iop/iop_intc.h>
namespace cosmic::iop {
    void IopINTC::iopCheck() {
        iop->intByINTC(ctrl && (stat & mask));
    }

    void IopINTC::assertIrq(i32 id) {
        stat |= 1 << id;
        iopCheck();
    }
    u32 IopINTC::readMask() {
        return mask;
    }
    u32 IopINTC::readStat() {
        return stat;
    }
    u32 IopINTC::readICtrl() {
        // Global interrupt disable
        u32 ic{ctrl};
        ctrl ^= ctrl;
        iopCheck();
        return ic;
    }
    void IopINTC::wrStat(u32 st) {
        stat &= st;
        iopCheck();
    }
    void IopINTC::wrMask(u32 m) {
        mask = m;
        iopCheck();
    }
    void IopINTC::wrCtrl(u32 ic) {
        ctrl = ic & 1;
        iopCheck();
    }
}