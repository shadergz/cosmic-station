#include <creeper/ee/mipsiv_cached.h>
#include <engine/ee_core.h>
namespace cosmic::creeper::ee {
    void MipsIvInterpreter::mult(Operands ops) {
        i32 fi{cpu->GPRs[ops.rs].swords[0]};
        i32 se{cpu->GPRs[ops.rt].swords[0]};
        i64 multi{fi * se};
        cpu->setLoHi(static_cast<u64>(multi));
        cpu->GPRs[ops.rd].sdw[0] = cpu->mulDivStorage[0];
    }
    void MipsIvInterpreter::multu(Operands ops) {
        u64 multi{cpu->GPRs[ops.rs].words[0] * cpu->GPRs[ops.rt].words[0]};
        cpu->setLoHi(multi);
        cpu->GPRs[ops.rd].dw[0] = static_cast<u64>(cpu->mulDivStorage[0]);
    }
    void MipsIvInterpreter::div(Operands ops) {
        i32 dividend{cpu->GPRs[ops.rs].swords[0]};
        i32 divisor{cpu->GPRs[ops.rt].swords[0]};
        if (dividend == 0x80000000 && divisor == 0xffffffff) {
            cpu->setLoHi(0x80000000, 0);
        } else if (divisor) {
            cpu->setLoHi(dividend / divisor, dividend % divisor);
        } else {
            if (dividend >= 0)
                cpu->setLoHi(-1, dividend);
            else
                cpu->setLoHi(1, dividend);
        }
    }
    void MipsIvInterpreter::divu(Operands ops) {
        i32 dividend{cpu->GPRs[ops.rs].swords[0]};
        i32 divisor{cpu->GPRs[ops.rt].swords[0]};
        if (divisor) {
            cpu->setLoHi(dividend / divisor, dividend % divisor);
        } else {
            cpu->setLoHi(-1, dividend);
        }
    }

    void MipsIvInterpreter::add(Operands ops) {
        cpu->GPRs[ops.rd].sdw[0] = cpu->GPRs[ops.rs].swords[0] + cpu->GPRs[ops.rt].swords[0];
    }
    void MipsIvInterpreter::addu(Operands ops) {
        cpu->GPRs[ops.rd].dw[0] = cpu->GPRs[ops.rs].words[0] + cpu->GPRs[ops.rt].words[0];
    }
    void MipsIvInterpreter::sub(Operands ops) {
        cpu->GPRs[ops.rd].sdw[0] = cpu->GPRs[ops.rs].swords[0] - cpu->GPRs[ops.rt].swords[0];
    }
    void MipsIvInterpreter::subu(Operands ops) {
        cpu->GPRs[ops.rd].dw[0] = cpu->GPRs[ops.rs].words[0] - cpu->GPRs[ops.rt].words[0];
    }
    void MipsIvInterpreter::dadd(Operands ops) {
        cpu->GPRs[ops.rd].sdw[0] = cpu->GPRs[ops.rs].sdw[0] + cpu->GPRs[ops.rt].sdw[0];
    }
    void MipsIvInterpreter::daddu(Operands ops) {
        cpu->GPRs[ops.rd].dw[0] = cpu->GPRs[ops.rs].dw[0] + cpu->GPRs[ops.rt].dw[0];
    }
    void MipsIvInterpreter::dsub(Operands ops) {
        cpu->GPRs[ops.rd].sdw[0] = cpu->GPRs[ops.rs].sdw[0] - cpu->GPRs[ops.rt].sdw[0];
    }
    void MipsIvInterpreter::dsubu(Operands ops) {
        cpu->GPRs[ops.rd].dw[0] = cpu->GPRs[ops.rs].dw[0] - cpu->GPRs[ops.rt].dw[0];
    }
    void MipsIvInterpreter::srav(Operands ops) {
        // Shifting by a non immediate value (GPRs)
        i64* const shiftTo{cpu->gprAt<i64>(ops.rd)};
        *shiftTo = cpu->GPRs[ops.rt].swords[0] >> (cpu->GPRs[ops.rs].sdw[0] & 0x1f);
    }

    void MipsIvInterpreter::ivXor(Operands ops) {
        cpu->GPRs[ops.rd].dw[0] =
            (cpu->GPRs[ops.rs].dw[0]) ^
            (cpu->GPRs[ops.rt].dw[0]);
    }
    void MipsIvInterpreter::slt(Operands ops) {
        cpu->GPRs[ops.rd].dw[0] =
            cpu->GPRs[ops.rs].sdw[0] < cpu->GPRs[ops.rt].sdw[0];
    }
    void MipsIvInterpreter::sll(Operands ops) {
        if (ops.rt == 0)
            return;
        u8 shift{static_cast<u8>((ops.inst >> 6) & 0x1f)};
        i64* const shiftTo{cpu->gprAt<i64>(ops.rd)};
        *shiftTo = static_cast<i32>(cpu->GPRs[ops.rt].words[0] << shift);
    }
    void MipsIvInterpreter::srl(Operands ops) {
        u8 right{static_cast<u8>((ops.inst >> 6) & 0x1f)};
        i64* const shiftTo{cpu->gprAt<i64>(ops.rd)};
        *shiftTo = static_cast<i32>(cpu->GPRs[ops.rt].words[0] >> right);
    }
    void MipsIvInterpreter::sra(Operands ops) {
        i8 withBitSet{static_cast<i8>((ops.inst >> 6) & 0x1f)};
        i64* const shiftTo{cpu->gprAt<i64>(ops.rd)};
        *shiftTo = cpu->GPRs[ops.rt].swords[0] >> withBitSet;
    }
    void MipsIvInterpreter::sllv(Operands ops) {
        // Shifting by a non immediate value (GPRs)
        i64* const shiftTo{cpu->gprAt<i64>(ops.rd)};
        *shiftTo = static_cast<i32>(cpu->GPRs[ops.rt].words[0] <<
            (cpu->GPRs[ops.rs].b[0] & 0x1f));
    }
    void MipsIvInterpreter::srlv(Operands ops) {
        // Shifting by a non immediate value (GPRs)
        i64* const shiftTo{cpu->gprAt<i64>(ops.rd)};
        *shiftTo = static_cast<i32>(cpu->GPRs[ops.rt].words[0] >>
            (cpu->GPRs[ops.rs].b[0] & 0x1f));
    }
}