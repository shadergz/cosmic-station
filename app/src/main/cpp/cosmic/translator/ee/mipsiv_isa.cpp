#include <translator/ee/mipsiv_interpreter.h>
#include <engine/ee_core.h>
#include <console/backdoor.h>
#include <vm/emu_vm.h>
namespace cosmic::translator::ee {
    void MipsIvInterpreter::addi(Operands ops) {
        mainMips->GPRs[ops.sec].words[0] = ops.operation.pa16[0] +
            mainMips->GPRs[ops.thi].words[0];
    }

    void MipsIvInterpreter::slti(Operands ops) {
        u8 cmp{mainMips->GPRs[ops.thi].hw[0] < (ops.operation.sins & 0xffff)};
        mainMips->GPRs[ops.sec].dw[0] = cmp;
    }

    void MipsIvInterpreter::sw(Operands ops) {
        // The 16-bit signed offset is added to the contents of GPR base to form the effective address
        u32 stAddr{mainMips->GPRs[ops.thi].words[0] + ops.operation.inst & 0xffff};
        mainMips->mipsWrite(stAddr, mainMips->GPRs[ops.sec].words[0]);
    }

    // if (cond < {0, null}) ...
    void MipsIvInterpreter::bltzal(Operands ops) {
        // With the 18-bit signed instruction offset, the conditional branch range is ± 128 KBytes
        i32 jump{static_cast<i32>(ops.operation.pa16[0] << 2)};
        // GPR[31] ← PC + 8
        *mainMips->gprAt<u32>(engine::$ra) = *mainMips->lastPc + 8;
        mainMips->branchByCondition(mainMips->GPRs[ops.thi].dw[0] < 0, jump);
    }

    void MipsIvInterpreter::bne(Operands ops) {
        const u64 cond1{mainMips->GPRs[ops.thi].dw[0]};
        const u64 cond2{mainMips->GPRs[ops.sec].dw[0]};
        mainMips->branchByCondition(cond1 != cond2, (ops.operation.sins & 0xffff) << 2);
    }
    void MipsIvInterpreter::bgez(Operands ops) {
        i32 br{(ops.operation.sins & 0xffff) << 2};
        mainMips->branchByCondition(mainMips->GPRs[ops.thi].dw[0] >= 0, br);
    }
    void MipsIvInterpreter::bgezl(Operands ops) {
        const i32 br{(ops.operation.sins & 0xffff) << 2};
        mainMips->branchOnLikely(mainMips->GPRs[ops.thi].dw[0] >= 0, br);
    }
    void MipsIvInterpreter::bgezall(Operands ops) {
        // Place the return address link in GPR 31
        mainMips->GPRs[engine::$ra].words[0] = *mainMips->eePc + 8;
        u8 cmp{mainMips->GPRs[ops.thi].dw[0] >= 0};
        u16 imm{static_cast<u16>((ops.operation.sins & 0xffff) << 2)};
        mainMips->branchOnLikely(cmp, imm);
    }
    void MipsIvInterpreter::mtsab(Operands ops) {
        u32 sabbath{mainMips->GPRs[ops.thi].words[0]};
        u16 black{static_cast<u16>(ops.operation.sins & 0xffff)};
        sabbath = (sabbath & 0xf) ^ (black & 0xf);
        mainMips->sa = sabbath;
    }
    void MipsIvInterpreter::mtsah(Operands ops) {
        u16 imm{static_cast<u16>(ops.operation.pa16[0])};
        const u32 value{(mainMips->GPRs[ops.thi].words[0] & 0x7) ^ (imm & 0x7)};
        mainMips->sa = value * 2;
    }

#define CALC_OFFSET(reg)\
    static_cast<u32>(ops.operation.ps16[0] + mainMips->GPRs[reg].swords[0])

#define EFFECTIVE_LOAD_REGS(reg, offset, from)\
    mainMips->GPRs[reg].dw[0] =\
        static_cast<u64>(mainMips->mipsRead<from>(offset))
#define SIGNED_EFFECTIVE_LOAD_REGS(reg, offset, from)\
    *reinterpret_cast<i64*>(mainMips->GPRs[reg].dw[0]) =\
        static_cast<i64>(mainMips->mipsRead<from>(offset))

    void MipsIvInterpreter::lb(Operands ops) {
        SIGNED_EFFECTIVE_LOAD_REGS(ops.fir, CALC_OFFSET(ops.sec), i32);
    }
    void MipsIvInterpreter::lh(Operands ops) {
        SIGNED_EFFECTIVE_LOAD_REGS(ops.fir, CALC_OFFSET(ops.sec), u16);
    }
    void MipsIvInterpreter::lw(Operands ops) {
        SIGNED_EFFECTIVE_LOAD_REGS(ops.fir, CALC_OFFSET(ops.sec), u32);
    }
    void MipsIvInterpreter::lbu(Operands ops) {
        EFFECTIVE_LOAD_REGS(ops.fir, CALC_OFFSET(ops.sec), u32);
    }
    void MipsIvInterpreter::lhu(Operands ops) {
        EFFECTIVE_LOAD_REGS(ops.fir, CALC_OFFSET(ops.sec), u16);
    }
    void MipsIvInterpreter::lwu(Operands ops) {
        EFFECTIVE_LOAD_REGS(ops.fir, CALC_OFFSET(ops.sec), u32);
    }

    void MipsIvInterpreter::ld(Operands ops) {
        EFFECTIVE_LOAD_REGS(ops.fir, CALC_OFFSET(ops.sec), u64);
    }
    void MipsIvInterpreter::sd(Operands ops) {
        mainMips->mipsWrite(CALC_OFFSET(ops.sec), mainMips->GPRs[ops.fir].dw[0]);
    }
    void MipsIvInterpreter::cache(Operands ops) {
        const i32 as{mainMips->GPRs[ops.sec].swords[0] + ops.operation.ps16[0]};
        switch (ops.operation.pa8[3]) {
        case 0x07:
            mainMips->ctrl0.invIndexed(static_cast<u32>(as));
            break;
        }
    }
    void MipsIvInterpreter::nop(Operands ops) {}
    // If the value in GPR rt is equal to zero, then the contents of GPR rs are placed into GPR rd
    // The zero value tested here is the “condition false” result from the SLT, SLTI, SLTU, and
    // SLTIU comparison instructions
    void MipsIvInterpreter::movz(Operands ops) {
        const u64 attr{mainMips->GPRs[ops.thi].dw[0]};
        if (!mainMips->GPRs[ops.sec].dw[0])
            mainMips->GPRs[ops.fir].dw[0] = attr;
    }
    void MipsIvInterpreter::movn(Operands ops) {
        if (mainMips->GPRs[ops.sec].dw[0])
            mainMips->GPRs[ops.fir].dw[0] = mainMips->GPRs[ops.thi].dw[0];
    }

    void MipsIvInterpreter::ivBreak(Operands ops) {
        mainMips->handleException(1, 0x80000180, 0x9);
    }
    void MipsIvInterpreter::ivSyscall(Operands ops) {
        mainMips->ctrl0.cause.exCode = 0x8;
        // We need to directly handle these syscall, instead of mainMips.chPc(0x80000180);
        auto vm{redBox->openVm()};
        vm->dealWithSyscalls();
        redBox->leaveVm(vm);
    }
}
