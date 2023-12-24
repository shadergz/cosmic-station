#pragma once
#include <creeper/inst_operands.h>
#include <creeper/fastmem.h>

#include <iop/iop_info.h>
namespace cosmic::vm {
    class EmuVm;
}
namespace cosmic::creeper::psx {
    class IopInterpreter : public iop::IopExecVe {
    public:
        IopInterpreter(raw_reference<iop::IoMipsCore> core);
        u32 executeCode() override;
        u32 execIo3(u32 opcode, std::array<u8, 3> opeRegs);
        u32 execCopRow(u32 opcode, std::array<u8, 3> opeRegs);
        u32 execIo3s(u32 opcode, std::array<u8, 3> opeRegs);
    private:
        void ioFuncHook(u32 pc);
        CachedFastPc fastPc;

        u32 fetchPcInst() override;
        raw_reference<vm::EmuVm> vm;
        void issueInterruptSignal();

        void sltiu(Operands ops);
        void ioSyscall(Operands ops);

        void mfhi(Operands ops);
        void mthi(Operands ops);
        void orMips(Operands ops);
        void xorMips(Operands ops);
        void nor(Operands ops);
        void lui(Operands ops);
        void ori(Operands ops);
        void jr(Operands ops);
        void beq(Operands ops);
        void lw(Operands ops);
        void andi(Operands ops);
        void addi(Operands ops); void addiu(Operands ops);
        void sw(Operands ops);

        void mfc(Operands ops);
        void mtc(Operands ops);
        void rfe(Operands ops);

        void bne(Operands ops); void blez(Operands ops);
    };
}
