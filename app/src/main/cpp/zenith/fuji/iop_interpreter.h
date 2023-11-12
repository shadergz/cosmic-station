#pragma once

#include <iop/iop_fuji.h>
#include <fuji/fuji_macros.h>

#define IvFuji3Impl(op) IvFuji3(IOPInterpreter::op)
#define IvFujiSpecialImpl(op) IvFujiSpecial(IOPInterpreter::op)

namespace zenith::fuji {
    enum IOPOpcodes {
        Mfhi = 0x10,
        Mthi = 0x11
    };

    class IOPInterpreter : public iop::IOPExecVE {
    public:
        IOPInterpreter(iop::IOMipsCore& core)
            : IOPExecVE(core) {}
        u32 executeCode() override;

    private:
        u32 fetchPcInst() override;

        IvFuji3(mfhi);
        IvFuji3(mthi);
    };
}
