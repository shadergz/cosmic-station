#pragma once

#include <common/types.h>
#include <iop/iop_core.h>
namespace cosmic::iop {
    class IopINTC {
    public:
        IopINTC(std::shared_ptr<IOMipsCore>& mips)
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
        std::shared_ptr<IOMipsCore> iop;
    };
}