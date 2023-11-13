#pragma once
#include <common/types.h>

namespace zenith::iop {
    class IOMipsCore;

    class IOPExecVE {
    public:
        IOPExecVE(IOMipsCore& mips)
            : ioMips(mips) {}

        virtual u32 executeCode() = 0;
        virtual u32 fetchPcInst() = 0;
        virtual ~IOPExecVE() = default;
    protected:
        IOMipsCore& ioMips;
    };
}
