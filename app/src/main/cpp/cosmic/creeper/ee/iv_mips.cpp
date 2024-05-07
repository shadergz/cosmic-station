#include <creeper/ee/iv_cached.h>
#include <engine/ee_core.h>
#include <vm/emu_vm.h>

namespace cosmic::creeper::ee {
    void MipsIvInterpreter::addi(Operands ops) {
        cpu->GPRs[ops.rt].words[0] = ops.pa16[0] + cpu->GPRs[ops.rs].words[0];
    }
    void MipsIvInterpreter::lui(Operands ops) {
        cpu->GPRs[ops.rt].dw[0] = static_cast<u64>((ops.sins & 0xffff) << 16);
    }

    void MipsIvInterpreter::slti(Operands ops) {
        u8 cmp{cpu->GPRs[ops.rs].hw[0] < (ops.sins & 0xffff)};
        cpu->GPRs[ops.rt].dw[0] = cmp;
    }

    void MipsIvInterpreter::sw(Operands ops) {
        // The 16-bit signed offset is added to the contents of GPR base to form the effective address
        u32 stAddr{cpu->GPRs[ops.rs].words[0] + ops.inst & 0xffff};
        cpu->mipsWrite(stAddr, cpu->GPRs[ops.rt].words[0]);
    }

    // if (cond < {0, null}) ...
    void MipsIvInterpreter::bltzal(Operands ops) {
        // With the 18-bit signed instruction offset, the conditional branch range is ± 128 KBytes
        i32 jump{static_cast<i32>(ops.pa16[0] << 2)};
        // GPR[31] ← PC + 8
        *cpu->gprAt<u32>(engine::$ra) = *cpu->lastPc + 8;
        cpu->branchByCondition(cpu->GPRs[ops.rs].dw[0] < 0, jump);
    }

    void MipsIvInterpreter::bne(Operands ops) {
        const u64 cond1{cpu->GPRs[ops.rs].dw[0]};
        const u64 cond2{cpu->GPRs[ops.rt].dw[0]};
        cpu->branchByCondition(cond1 != cond2, (ops.sins & 0xffff) << 2);
    }
    void MipsIvInterpreter::bgez(Operands ops) {
        i32 br{(ops.sins & 0xffff) << 2};
        cpu->branchByCondition(cpu->GPRs[ops.rs].dw[0] >= 0, br);
    }
    void MipsIvInterpreter::bgezl(Operands ops) {
        const i32 br{(ops.sins & 0xffff) << 2};
        cpu->branchOnLikely(cpu->GPRs[ops.rs].dw[0] >= 0, br);
    }
    void MipsIvInterpreter::bgezall(Operands ops) {
        // Place the return address link in GPR 31
        cpu->GPRs[engine::$ra].words[0] = *cpu->eePc + 8;
        u8 cmp{cpu->GPRs[ops.rs].dw[0] >= 0};
        u16 imm{static_cast<u16>((ops.sins & 0xffff) << 2)};
        cpu->branchOnLikely(cmp, imm);
    }
    void MipsIvInterpreter::mtsab(Operands ops) {
        u32 sabbath{cpu->GPRs[ops.rs].words[0]};
        u16 black{static_cast<u16>(ops.sins & 0xffff)};
        sabbath = (sabbath & 0xf) ^ (black & 0xf);
        cpu->sa = sabbath;
    }
    void MipsIvInterpreter::mtsah(Operands ops) {
        u16 imm{static_cast<u16>(ops.pa16[0])};
        const u32 value{(cpu->GPRs[ops.rs].words[0] & 0x7) ^ (imm & 0x7)};
        cpu->sa = value * 2;
    }

#define CALC_OFFSET(reg)\
    static_cast<u32>(ops.ps16[0] + cpu->GPRs[reg].swords[0])

#define EFFECTIVE_LOAD_REGS(reg, offset, from)\
    cpu->GPRs[reg].dw[0] =\
        static_cast<u64>(cpu->mipsRead<from>(offset))
#define SIGNED_EFFECTIVE_LOAD_REGS(reg, offset, from)\
    *reinterpret_cast<i64*>(cpu->GPRs[reg].dw[0]) =\
        static_cast<i64>(cpu->mipsRead<from>(offset))

    void MipsIvInterpreter::lb(Operands ops) {
        SIGNED_EFFECTIVE_LOAD_REGS(ops.rd, CALC_OFFSET(ops.rt), i32);
    }
    void MipsIvInterpreter::lh(Operands ops) {
        SIGNED_EFFECTIVE_LOAD_REGS(ops.rd, CALC_OFFSET(ops.rt), u16);
    }
    void MipsIvInterpreter::lw(Operands ops) {
        SIGNED_EFFECTIVE_LOAD_REGS(ops.rd, CALC_OFFSET(ops.rt), u32);
    }
    void MipsIvInterpreter::lbu(Operands ops) {
        EFFECTIVE_LOAD_REGS(ops.rd, CALC_OFFSET(ops.rt), u32);
    }
    void MipsIvInterpreter::lhu(Operands ops) {
        EFFECTIVE_LOAD_REGS(ops.rd, CALC_OFFSET(ops.rt), u16);
    }
    void MipsIvInterpreter::lwu(Operands ops) {
        EFFECTIVE_LOAD_REGS(ops.rd, CALC_OFFSET(ops.rt), u32);
    }

    void MipsIvInterpreter::ld(Operands ops) {
        EFFECTIVE_LOAD_REGS(ops.rd, CALC_OFFSET(ops.rt), u64);
    }
    void MipsIvInterpreter::sd(Operands ops) {
        cpu->mipsWrite(CALC_OFFSET(ops.rt), cpu->GPRs[ops.rd].dw[0]);
    }
    void MipsIvInterpreter::cache(Operands ops) {
        const i32 as{cpu->GPRs[ops.rs].swords[0] + ops.ps16[0]};
        if (ops.pa8[3] == 0x7) {
            control->invIndexed(static_cast<u32>(as));
        }
    }
    void MipsIvInterpreter::nop(Operands ops) {}
    // If the value in GPR rt is equal to zero, then the contents of GPR rs are placed into GPR rd
    // The zero value tested here is the “condition false” result from the SLT, SLTI, SLTU, and
    // SLTIU comparison instructions
    void MipsIvInterpreter::movz(Operands ops) {
        const u64 attr{cpu->GPRs[ops.rs].dw[0]};
        if (!cpu->GPRs[ops.rt].dw[0])
            cpu->GPRs[ops.rd].dw[0] = attr;
    }
    void MipsIvInterpreter::movn(Operands ops) {
        if (!ops.rd)
            return;
        if (cpu->GPRs[ops.rt].dw[0])
            cpu->GPRs[ops.rd].dw[0] = cpu->GPRs[ops.rs].dw[0];
    }
}
