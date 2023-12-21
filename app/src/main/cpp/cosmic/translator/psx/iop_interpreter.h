#pragma once
#include <translator/inst_operands.h>
#include <translator/fastmem.h>

#include <iop/iop_info.h>
namespace cosmic::translator::psx {
    class IopInterpreter : public iop::IopExecVe {
    public:
        IopInterpreter(raw_reference<iop::IoMipsCore> core) :
            IopExecVe(core) {}
        u32 executeCode() override;
        u32 execIo3(u32 opcode, std::array<u8, 3> opeRegs);
        u32 execCopRow(u32 opcode, std::array<u8, 3> opeRegs);
        u32 execIo3s(u32 opcode, std::array<u8, 3> opeRegs);
    private:
        void ioFuncHook(u32 pc);
        CachedFastPc fastPc;

        u32 fetchPcInst() override;
        void issueInterruptSignal();

        void sltiu(Operands ops);
        void ioSyscall(Operands ops);

        void mfhi(Operands ops);
        void mthi(Operands ops);
        void orSMips(Operands ops);
        void xorSMips(Operands ops);
        void nor(Operands ops);

        void mfc(Operands ops);
        void mtc(Operands ops);
        void rfe(Operands ops);

        void bne(Operands ops); void blez(Operands ops);
    };
}
