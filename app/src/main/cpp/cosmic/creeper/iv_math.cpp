#include <creeper/cached_blocks.h>
#include <engine/ee_core.h>
#include <console/backdoor.h>

namespace cosmic::creeper {
    void MipsIvInterpreter::mult(Operands ops) {
        auto fi{cpu->GPRs[ops.rs].swords[0]};
        auto se{cpu->GPRs[ops.rt].swords[0]};
        auto multi{fi * se};

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

#define DO_OP_IV(r, op)\
    r = RS_WORDS_S op RT_WORDS_S
#define DO_OP_IV_UNS(r, op)\
    r = RS_WORDS op RT_WORDS

    void MipsIvInterpreter::add(Operands ops) {
        DO_OP_IV(cpu->GPRs[ops.rd].sdw[0], +);
    }
    void MipsIvInterpreter::addu(Operands ops) {
        DO_OP_IV_UNS(cpu->GPRs[ops.rd].dw[0], +);
    }
    void MipsIvInterpreter::sub(Operands ops) {
        DO_OP_IV(cpu->GPRs[ops.rd].sdw[0], -);
    }
    void MipsIvInterpreter::subu(Operands ops) {
        DO_OP_IV_UNS(cpu->GPRs[ops.rd].dw[0], -);
    }

    void MipsIvInterpreter::ori(Operands ops) {
        RT_DW = (RS_DW) | (ops.inst & 0xffff);
    }
    void MipsIvInterpreter::xori(Operands ops) {
        RT_DW = (RS_DW) ^ (ops.inst & 0xffff);
    }
    void MipsIvInterpreter::slt(Operands ops) {
        cpu->GPRs[ops.rd].dw[0] =
            cpu->GPRs[ops.rs].sdw[0] < cpu->GPRs[ops.rt].sdw[0];
    }

#define PERFORM_WRITE_AT(r, op)\
    auto const address{cpu->gprAt<i64>(r)};\
    *address = static_cast<i32>(RT_WORDS_S op static_cast<u8>((ops.inst >> 6) & 0x1f))

    void MipsIvInterpreter::sll(Operands ops) {
        if (ops.rt) {
            PERFORM_WRITE_AT(ops.rd, <<);
        }
    }
    void MipsIvInterpreter::srl(Operands ops) {
        if (ops.rt) {
            PERFORM_WRITE_AT(ops.rd, >>);
        }
    }

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