#pragma once

#include <common/types.h>
#include <iop/iop_core.h>
namespace zenith::iop {
    class IopINTC {
    public:
        IopINTC(std::shared_ptr<IOMipsCore>& mips)
            : iop(mips) {}
        void iopCheck();

        u32 stat, mask, ctrl;
    private:
        std::shared_ptr<IOMipsCore> iop;
    };
}