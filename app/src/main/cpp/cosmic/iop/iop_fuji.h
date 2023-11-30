#pragma once
#include <common/types.h>

#include <iop/iop_info.h>
namespace cosmic::iop {
    class IoMipsCore;

    class IopExecVE {
    public:
        IopExecVE(IoMipsCore& mips)
            : ioMips(mips) {}

        virtual u32 executeCode() = 0;
        virtual u32 fetchPcInst() = 0;
        virtual ~IopExecVE() = default;
    protected:
        IoMipsCore& ioMips;
    };
}
