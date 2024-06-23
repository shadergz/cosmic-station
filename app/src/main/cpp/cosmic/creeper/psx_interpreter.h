#pragma once
#include <creeper/inst_operands.h>
#include <creeper/fastmem.h>

#include <iop/iop_info.h>
namespace cosmic::vm {
    class EmuVm;
}
namespace cosmic::creeper {
    class IopInterpreter : public iop::IopExecVe {
    public:
        IopInterpreter(Wrapper<iop::IoMipsCore> core);
        u32 executeCode() override;
        u32 execPsx(u32 opcode, std::array<u8, 3> opeRegs);
        u32 execCop(u32 opcode, std::array<u8, 3> opeRegs);
        u32 execSpecial(u32 opcode, std::array<u8, 3> opeRegs);
    private:
        void ioFuncHook(u32 pc);
        CachedFastPc fastPc;

        u32 fetchPcInst() override;
        Wrapper<vm::EmuVm> vm;
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
