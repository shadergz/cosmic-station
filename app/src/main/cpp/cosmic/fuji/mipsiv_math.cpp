#include <fuji/mipsiv_interpreter.h>
#include <engine/ee_core.h>
namespace cosmic::fuji {
    IV_FUJI_SUPER_ASM(mult) {
        i32 fi{mainMips.GPRs[ops.thi].swords[0]};
        i32 se{mainMips.GPRs[ops.sec].swords[0]};
        i64 multi{fi * se};
        mainMips.setLoHi(static_cast<u64>(multi));
        mainMips.GPRs[ops.fir].sdw[0] = mainMips.mulDivStorage[0];
    }
    IV_FUJI_SUPER_ASM(multu) {
        u64 multi{mainMips.GPRs[ops.thi].words[0] * mainMips.GPRs[ops.sec].words[0]};
        mainMips.setLoHi(multi);
        mainMips.GPRs[ops.fir].dw[0] = static_cast<u64>(mainMips.mulDivStorage[0]);
    }
    IV_FUJI_SUPER_ASM(div) {
        i32 dividend{mainMips.GPRs[ops.thi].swords[0]};
        i32 divisor{mainMips.GPRs[ops.sec].swords[0]};
        if (dividend == 0x80000000 && divisor == 0xffffffff) {
            mainMips.setLoHi(0x80000000, 0);
        } else if (divisor) {
            mainMips.setLoHi(dividend / divisor, dividend % divisor);
        } else {
            if (dividend >= 0)
                mainMips.setLoHi(-1, dividend);
            else
                mainMips.setLoHi(1, dividend);
        }
    }
    IV_FUJI_SUPER_ASM(divu) {
        i32 dividend{mainMips.GPRs[ops.thi].swords[0]};
        i32 divisor{mainMips.GPRs[ops.sec].swords[0]};
        if (divisor) {
            mainMips.setLoHi(dividend / divisor, dividend % divisor);
        } else {
            mainMips.setLoHi(-1, dividend);
        }
    }

    IV_FUJI_SUPER_ASM(add) {
        mainMips.GPRs[ops.fir].sdw[0] = mainMips.GPRs[ops.thi].swords[0] + mainMips.GPRs[ops.sec].swords[0];
    }
    IV_FUJI_SUPER_ASM(addu) {
        mainMips.GPRs[ops.fir].dw[0] = mainMips.GPRs[ops.thi].words[0] + mainMips.GPRs[ops.sec].words[0];
    }
    IV_FUJI_SUPER_ASM(sub) {
        mainMips.GPRs[ops.fir].sdw[0] = mainMips.GPRs[ops.thi].swords[0] - mainMips.GPRs[ops.sec].swords[0];
    }
    IV_FUJI_SUPER_ASM(subu) {
        mainMips.GPRs[ops.fir].dw[0] = mainMips.GPRs[ops.thi].words[0] - mainMips.GPRs[ops.sec].words[0];
    }
    IV_FUJI_SUPER_ASM(dadd) {
        mainMips.GPRs[ops.fir].sdw[0] = mainMips.GPRs[ops.thi].sdw[0] + mainMips.GPRs[ops.sec].sdw[0];
    }
    IV_FUJI_SUPER_ASM(daddu) {
        mainMips.GPRs[ops.fir].dw[0] = mainMips.GPRs[ops.thi].dw[0] + mainMips.GPRs[ops.sec].dw[0];
    }
    IV_FUJI_SUPER_ASM(dsub) {
        mainMips.GPRs[ops.fir].sdw[0] = mainMips.GPRs[ops.thi].sdw[0] - mainMips.GPRs[ops.sec].sdw[0];
    }
    IV_FUJI_SUPER_ASM(dsubu) {
        mainMips.GPRs[ops.fir].dw[0] = mainMips.GPRs[ops.thi].dw[0] - mainMips.GPRs[ops.sec].dw[0];
    }
    IV_FUJI_SUPER_ASM(srav) {
        // Shifting by a non immediate value (GPRs)
        i64* const shiftTo{mainMips.gprAt<i64>(ops.fir)};
        *shiftTo = mainMips.GPRs[ops.sec].swords[0] >>
            (mainMips.GPRs[ops.thi].sdw[0] & 0x1f);
    }
    IV_FUJI_SUPER_ASM(ivXor) {
        mainMips.GPRs[ops.fir].dw[0] =
            (mainMips.GPRs[ops.thi].dw[0]) ^
            (mainMips.GPRs[ops.sec].dw[0]);
    }
    IV_FUJI_SUPER_ASM(slt) {
        mainMips.GPRs[ops.fir].dw[0] =
            mainMips.GPRs[ops.thi].sdw[0] < mainMips.GPRs[ops.sec].sdw[0];
    }
    IV_FUJI_SUPER_ASM(sll) {
        u8 shift{static_cast<u8>((ops.operation.inst >> 6) & 0x1f)};
        i64* const shiftTo{mainMips.gprAt<i64>(ops.fir)};
        *shiftTo = static_cast<i32>(mainMips.GPRs[ops.sec].words[0] << shift);
    }
    IV_FUJI_SUPER_ASM(srl) {
        u8 right{static_cast<u8>((ops.operation.inst >> 6) & 0x1f)};
        i64* const shiftTo{mainMips.gprAt<i64>(ops.fir)};
        *shiftTo = static_cast<i32>(mainMips.GPRs[ops.sec].words[0] >> right);
    }
    IV_FUJI_SUPER_ASM(sra) {
        i8 withBitSet{static_cast<i8>((ops.operation.inst >> 6) & 0x1f)};
        i64* const shiftTo{mainMips.gprAt<i64>(ops.fir)};
        *shiftTo = mainMips.GPRs[ops.sec].swords[0] >> withBitSet;
    }
    IV_FUJI_SUPER_ASM(sllv) {
        // Shifting by a non immediate value (GPRs)
        i64* const shiftTo{mainMips.gprAt<i64>(ops.fir)};
        *shiftTo = static_cast<i32>(mainMips.GPRs[ops.sec].words[0] <<
            (mainMips.GPRs[ops.thi].bytes[0] & 0x1f));
    }
    IV_FUJI_SUPER_ASM(srlv) {
        // Shifting by a non immediate value (GPRs)
        i64* const shiftTo{mainMips.gprAt<i64>(ops.fir)};
        *shiftTo = static_cast<i32>(mainMips.GPRs[ops.sec].words[0] >>
            (mainMips.GPRs[ops.thi].bytes[0] & 0x1f));
    }
}