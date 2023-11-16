#pragma once

#include <iop/iop_fuji.h>
#include <fuji/fuji_macros.h>

#define IvFuji3Impl(op) IvFuji3(IOPInterpreter::op)
#define IvFujiSpecialImpl(op) IvFujiSpecial(IOPInterpreter::op)

namespace zenith::fuji {
    enum IOPOpcodes {
        SpecialOp = 0x0,
        CopMfc = 0x0,
        CopMtc = 0x4,
        Slti = 0x0a,
        Sltiu = 0x0b,
        SpecialMfhi = 0x10,
        CopRfe = 0x10,
        SpecialMthi = 0x11,
        SpecialOr = 0x25,
        SpecialXor = 0x26,
        SpecialNor = 0x27
    };

    class IOPInterpreter : public iop::IOPExecVE {
    public:
        IOPInterpreter(iop::IOMipsCore& core)
            : IOPExecVE(core) {}
        u32 executeCode() override;
        u32 execIO3(u32 opcode, std::span<u32*> opeRegs);
        u32 execCopRow(u32 opcode, std::span<u32*> opeRegs);
        u32 execIO3S(u32 opcode, std::span<u32*> opeRegs);

    private:
        u32 fetchPcInst() override;

        IvFuji3(sltBy);

        IvFujiSpecial(mfhi);
        IvFujiSpecial(mthi);
        IvFujiSpecial(orSMips);
        IvFujiSpecial(xorSMips);
        IvFujiSpecial(nor);

        IvFuji3(mfc);
        IvFuji3(mtc);
        IvFuji3(rfe);
    };
}
