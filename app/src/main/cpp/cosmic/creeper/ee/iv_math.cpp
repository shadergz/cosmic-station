#include <creeper/ee/cached_blocks.h>
#include <creeper/ee/fast_macros.h>
#include <engine/ee_core.h>
#include <console/backdoor.h>

namespace cosmic::creeper::ee {
    void MipsIvInterpreter::mult(Operands ops) {
        i32 fi{cpu->GPRs[ops.rs].swords[0]};
        i32 se{cpu->GPRs[ops.rt].swords[0]};
        i64 multi{fi * se};
        cpu->setLoHi(static_cast<u64>(multi));
        cpu->GPRs[ops.rd].sdw[0] = cpu->mulDivStorage[0];
    }
    void MipsIvInterpreter::multu(Operands ops) {
        auto multi{cpu->GPRs[ops.rs].words[0] * cpu->GPRs[ops.rt].words[0]};
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

#define DECLARE_MATH_IV(name, op)\
    void MipsIvInterpreter::name(Operands ops) {\
        cpu->GPRs[ops.rd].sdw[0] = RS_WORDS_S op RT_WORDS_S;\
    }
#define DECLARE_MATH_IV_UNS(name, op)\
    void MipsIvInterpreter::name(Operands ops) {\
        cpu->GPRs[ops.rd].dw[0] = RS_WORDS op RT_WORDS;\
    }
    DECLARE_MATH_IV(add, +)
    DECLARE_MATH_IV(sub, -)
    DECLARE_MATH_IV_UNS(addu, +)
    DECLARE_MATH_IV_UNS(subu, -)

#define IV_OP_I(op)\
    RD_DW = (RS_DW) op (RT_DW)

#define DECLARE_FUNC_IV(name, op)\
    void MipsIvInterpreter::name(Operands ops) {\
        IV_OP_I(op);\
    }
    DECLARE_FUNC_IV(ori, |)
    DECLARE_FUNC_IV(xori, ^)

    void MipsIvInterpreter::slt(Operands ops) {
        cpu->GPRs[ops.rd].dw[0] =
            cpu->GPRs[ops.rs].sdw[0] < cpu->GPRs[ops.rt].sdw[0];
    }

#define DECLARE_FUNC_SHIFT(name, op)\
    void MipsIvInterpreter::name(Operands ops) {\
        if (ops.rt == 0)\
            return;\
        auto const address{cpu->gprAt<i64>(ops.rd)};\
        *address = static_cast<i32>(RT_WORDS op static_cast<u8>((ops.inst >> 6) & 0x1f));\
    }
    DECLARE_FUNC_SHIFT(sll, <<)
    DECLARE_FUNC_SHIFT(srl, >>)

    void MipsIvInterpreter::sra(Operands ops) {
        auto withBitSet{static_cast<i8>((ops.inst >> 6) & 0x1f)};
        auto const address{cpu->gprAt<i64>(ops.rd)};
        *address = cpu->GPRs[ops.rt].swords[0] >> withBitSet;
    }
    void MipsIvInterpreter::sllv(Operands ops) {
        // Shifting by a non immediate value (GPRs)
        auto const address{cpu->gprAt<i64>(ops.rd)};
        *address = static_cast<i32>(cpu->GPRs[ops.rt].words[0] <<
            (cpu->GPRs[ops.rs].b[0] & 0x1f));
    }
    void MipsIvInterpreter::srlv(Operands ops) {
        // Shifting by a non immediate value (GPRs)
        auto const address{cpu->gprAt<i64>(ops.rd)};
        *address = static_cast<i32>(cpu->GPRs[ops.rt].words[0] >>
            (cpu->GPRs[ops.rs].b[0] & 0x1f));
    }
}