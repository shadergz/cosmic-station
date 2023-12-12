#pragma once

#include <common/types.h>
#include <iop/iop_core.h>
namespace cosmic::iop {
    class IopIntC {
    public:
        IopIntC(std::shared_ptr<IoMipsCore>& mips)
            : iop(mips) {}
        void iopCheck();

        void assertIrq(i32 id);
        u32 readStat();
        u32 readMask();
        u32 readICtrl();

        void wrStat(u32 st);
        void wrMask(u32 m);
        void wrCtrl(u32 ic);
        u32 stat, mask, ctrl;
    private:
        std::shared_ptr<IoMipsCore> iop;
    };
}