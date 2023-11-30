#pragma once

#include <fuji/fuji_common.h>
#include <iop/iop_fuji.h>

#define IV_FUJI_IOP_ASM(op) IV_FUJI_OP(IOPInterpreter::op)
namespace cosmic::fuji {
    class IOPInterpreter : public iop::IopExecVE {
    public:
        IOPInterpreter(iop::IoMipsCore& core)
            : IopExecVE(core) {}
        u32 executeCode() override;
        u32 execIO3(u32 opcode, std::array<u8, 3> opeRegs);
        u32 execCopRow(u32 opcode, std::array<u8, 3> opeRegs);
        u32 execIO3S(u32 opcode, std::array<u8, 3> opeRegs);
    private:
        u32 fetchPcInst() override;
        void issueInterruptSignal();

        IV_FUJI_OP(sltAny);
        IV_FUJI_OP(ioSyscall);

        IV_FUJI_OP(mfhi);
        IV_FUJI_OP(mthi);
        IV_FUJI_OP(orSMips);
        IV_FUJI_OP(xorSMips);
        IV_FUJI_OP(nor);

        IV_FUJI_OP(mfc);
        IV_FUJI_OP(mtc);
        IV_FUJI_OP(rfe);

        IV_FUJI_OP(bne); IV_FUJI_OP(blez);
    };
}
