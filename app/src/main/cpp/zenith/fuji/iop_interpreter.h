#pragma once

#include <iop/iop_fuji.h>
#include <fuji/fuji_common.h>

#define IvFujiIopAsm(op) IvFujiOp(IOPInterpreter::op)
namespace zenith::fuji {
    enum IOPOpcodes {
        SpecialOp = 0x0,
        CopMfc = 0x0,
        CopMtc = 0x4,
        IopSlti = 0x0a,
        Sltiu = 0x0b,
        IopSpecialSyscall = 0xc,
        SpecialMfhi = 0x10,
        CopRfe = 0x10,
        SpecialMthi = 0x11,
        SpecialOr = 0x25,
        IopSpecialXor = 0x26,
        SpecialNor = 0x27
    };

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
