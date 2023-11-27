#pragma once

#include <fuji/fuji_common.h>
#include <iop/iop_fuji.h>

#define IvFujiIopAsm(op) IvFujiOp(IOPInterpreter::op)
namespace cosmic::fuji {
    class IOPInterpreter : public iop::IOPExecVE {
    public:
        IOPInterpreter(iop::IOMipsCore& core)
            : IOPExecVE(core) {}
        u32 executeCode() override;
        u32 execIO3(u32 opcode, std::array<u8, 3> opeRegs);
        u32 execCopRow(u32 opcode, std::array<u8, 3> opeRegs);
        u32 execIO3S(u32 opcode, std::array<u8, 3> opeRegs);

    private:
        u32 fetchPcInst() override;

        IvFujiOp(sltBy);
        IvFujiOp(syscall);

        IvFujiOp(mfhi);
        IvFujiOp(mthi);
        IvFujiOp(orSMips);
        IvFujiOp(xorSMips);
        IvFujiOp(nor);

        IvFujiOp(mfc);
        IvFujiOp(mtc);
        IvFujiOp(rfe);
    };
}
