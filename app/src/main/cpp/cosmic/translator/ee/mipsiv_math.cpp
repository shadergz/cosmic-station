#include <translator/ee/mipsiv_interpreter.h>
#include <engine/ee_core.h>
namespace cosmic::translator::ee {
    void MipsIvInterpreter::mult(Operands ops) {
        i32 fi{mainMips->GPRs[ops.rs].swords[0]};
        i32 se{mainMips->GPRs[ops.rt].swords[0]};
        i64 multi{fi * se};
        mainMips->setLoHi(static_cast<u64>(multi));
        mainMips->GPRs[ops.rd].sdw[0] = mainMips->mulDivStorage[0];
    }
    void MipsIvInterpreter::multu(Operands ops) {
        u64 multi{mainMips->GPRs[ops.rs].words[0] * mainMips->GPRs[ops.rt].words[0]};
        mainMips->setLoHi(multi);
        mainMips->GPRs[ops.rd].dw[0] = static_cast<u64>(mainMips->mulDivStorage[0]);
    }
    void MipsIvInterpreter::div(Operands ops) {
        i32 dividend{mainMips->GPRs[ops.rs].swords[0]};
        i32 divisor{mainMips->GPRs[ops.rt].swords[0]};
        if (dividend == 0x80000000 && divisor == 0xffffffff) {
            mainMips->setLoHi(0x80000000, 0);
        } else if (divisor) {
            mainMips->setLoHi(dividend / divisor, dividend % divisor);
        } else {
            if (dividend >= 0)
                mainMips->setLoHi(-1, dividend);
            else
                mainMips->setLoHi(1, dividend);
        }
    }
    void MipsIvInterpreter::divu(Operands ops) {
        i32 dividend{mainMips->GPRs[ops.rs].swords[0]};
        i32 divisor{mainMips->GPRs[ops.rt].swords[0]};
        if (divisor) {
            mainMips->setLoHi(dividend / divisor, dividend % divisor);
        } else {
            mainMips->setLoHi(-1, dividend);
        }
    }

    void MipsIvInterpreter::add(Operands ops) {
        mainMips->GPRs[ops.rd].sdw[0] = mainMips->GPRs[ops.rs].swords[0] + mainMips->GPRs[ops.rt].swords[0];
    }
    void MipsIvInterpreter::addu(Operands ops) {
        mainMips->GPRs[ops.rd].dw[0] = mainMips->GPRs[ops.rs].words[0] + mainMips->GPRs[ops.rt].words[0];
    }
    void MipsIvInterpreter::sub(Operands ops) {
        mainMips->GPRs[ops.rd].sdw[0] = mainMips->GPRs[ops.rs].swords[0] - mainMips->GPRs[ops.rt].swords[0];
    }
    void MipsIvInterpreter::subu(Operands ops) {
        mainMips->GPRs[ops.rd].dw[0] = mainMips->GPRs[ops.rs].words[0] - mainMips->GPRs[ops.rt].words[0];
    }
    void MipsIvInterpreter::dadd(Operands ops) {
        mainMips->GPRs[ops.rd].sdw[0] = mainMips->GPRs[ops.rs].sdw[0] + mainMips->GPRs[ops.rt].sdw[0];
    }
    void MipsIvInterpreter::daddu(Operands ops) {
        mainMips->GPRs[ops.rd].dw[0] = mainMips->GPRs[ops.rs].dw[0] + mainMips->GPRs[ops.rt].dw[0];
    }
    void MipsIvInterpreter::dsub(Operands ops) {
        mainMips->GPRs[ops.rd].sdw[0] = mainMips->GPRs[ops.rs].sdw[0] - mainMips->GPRs[ops.rt].sdw[0];
    }
    void MipsIvInterpreter::dsubu(Operands ops) {
        mainMips->GPRs[ops.rd].dw[0] = mainMips->GPRs[ops.rs].dw[0] - mainMips->GPRs[ops.rt].dw[0];
    }
    void MipsIvInterpreter::srav(Operands ops) {
        // Shifting by a non immediate value (GPRs)
        i64* const shiftTo{mainMips->gprAt<i64>(ops.rd)};
        *shiftTo = mainMips->GPRs[ops.rt].swords[0] >> (mainMips->GPRs[ops.rs].sdw[0] & 0x1f);
    }

    void MipsIvInterpreter::ivXor(Operands ops) {
        mainMips->GPRs[ops.rd].dw[0] =
            (mainMips->GPRs[ops.rs].dw[0]) ^
            (mainMips->GPRs[ops.rt].dw[0]);
    }
    void MipsIvInterpreter::slt(Operands ops) {
        mainMips->GPRs[ops.rd].dw[0] =
            mainMips->GPRs[ops.rs].sdw[0] < mainMips->GPRs[ops.rt].sdw[0];
    }
    void MipsIvInterpreter::sll(Operands ops) {
        u8 shift{static_cast<u8>((ops.operation.inst >> 6) & 0x1f)};
        i64* const shiftTo{mainMips->gprAt<i64>(ops.rd)};
        *shiftTo = static_cast<i32>(mainMips->GPRs[ops.rt].words[0] << shift);
    }
    void MipsIvInterpreter::srl(Operands ops) {
        u8 right{static_cast<u8>((ops.operation.inst >> 6) & 0x1f)};
        i64* const shiftTo{mainMips->gprAt<i64>(ops.rd)};
        *shiftTo = static_cast<i32>(mainMips->GPRs[ops.rt].words[0] >> right);
    }
    void MipsIvInterpreter::sra(Operands ops) {
        i8 withBitSet{static_cast<i8>((ops.operation.inst >> 6) & 0x1f)};
        i64* const shiftTo{mainMips->gprAt<i64>(ops.rd)};
        *shiftTo = mainMips->GPRs[ops.rt].swords[0] >> withBitSet;
    }
    void MipsIvInterpreter::sllv(Operands ops) {
        // Shifting by a non immediate value (GPRs)
        i64* const shiftTo{mainMips->gprAt<i64>(ops.rd)};
        *shiftTo = static_cast<i32>(mainMips->GPRs[ops.rt].words[0] <<
            (mainMips->GPRs[ops.rs].b[0] & 0x1f));
    }
    void MipsIvInterpreter::srlv(Operands ops) {
        // Shifting by a non immediate value (GPRs)
        i64* const shiftTo{mainMips->gprAt<i64>(ops.rd)};
        *shiftTo = static_cast<i32>(mainMips->GPRs[ops.rt].words[0] >>
            (mainMips->GPRs[ops.rs].b[0] & 0x1f));
    }
}