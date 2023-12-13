// SPDX-short-identifier: MIT, Version N/A
// This file is protected by the MIT license (please refer to LICENSE.md before making any changes, copying, or redistributing this software)
#include <iop/iop_intc.h>
namespace cosmic::iop {
    void IopIntC::iopCheck() {
        iop->intByIntC(ctrl && (stat & mask));
    }

    void IopIntC::assertIrq(i32 id) {
        stat |= 1 << id;
        iopCheck();
    }
    u32 IopIntC::readMask() {
        return mask;
    }
    u32 IopIntC::readStat() {
        return stat;
    }
    u32 IopIntC::readICtrl() {
        // Global interrupt disable
        u32 ic{ctrl};
        ctrl ^= ctrl;
        iopCheck();
        return ic;
    }
    void IopIntC::wrStat(u32 st) {
        stat &= st;
        iopCheck();
    }
    void IopIntC::wrMask(u32 m) {
        mask = m;
        iopCheck();
    }
    void IopIntC::wrCtrl(u32 ic) {
        ctrl = ic & 1;
        iopCheck();
    }
}