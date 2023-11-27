#include <fuji/mipsiv_interpreter.h>
#include <eeiv/ee_engine.h>
namespace cosmic::fuji {
    IvFujiSuperAsm(mult) {
        i32 fi{mainMips.GPRs[ops.thi].swords[0]};
        i32 se{mainMips.GPRs[ops.sec].swords[0]};
        i64 multi{fi * se};
        mainMips.setLoHi(static_cast<u64>(multi));
        mainMips.GPRs[ops.fir].sdw[0] = mainMips.mulDivStorage[0];
    }
    IvFujiSuperAsm(multu) {
        u64 multi{mainMips.GPRs[ops.thi].words[0] * mainMips.GPRs[ops.sec].words[0]};
        mainMips.setLoHi(multi);
        mainMips.GPRs[ops.fir].dw[0] = static_cast<u64>(mainMips.mulDivStorage[0]);
    }
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshift-count-overflow"
    IvFujiSuperAsm(div) {
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
    IvFujiSuperAsm(divu) {
        i32 dividend{mainMips.GPRs[ops.thi].swords[0]};
        i32 divisor{mainMips.GPRs[ops.sec].swords[0]};
        if (divisor) {
            mainMips.setLoHi(dividend / divisor, dividend % divisor);
        } else {
            mainMips.setLoHi(-1, dividend);
        }
    }
#pragma clang diagnostic pop
    IvFujiSuperAsm(add) {
        mainMips.GPRs[ops.fir].sdw[0] = mainMips.GPRs[ops.thi].swords[0] + mainMips.GPRs[ops.sec].swords[0];
    }
    IvFujiSuperAsm(addu) {
        mainMips.GPRs[ops.fir].dw[0] = mainMips.GPRs[ops.thi].words[0] + mainMips.GPRs[ops.sec].words[0];
    }
    IvFujiSuperAsm(sub) {
        mainMips.GPRs[ops.fir].sdw[0] = mainMips.GPRs[ops.thi].swords[0] - mainMips.GPRs[ops.sec].swords[0];
    }
    IvFujiSuperAsm(subu) {
        mainMips.GPRs[ops.fir].dw[0] = mainMips.GPRs[ops.thi].words[0] - mainMips.GPRs[ops.sec].words[0];
    }
    IvFujiSuperAsm(dadd) {
        mainMips.GPRs[ops.fir].sdw[0] = mainMips.GPRs[ops.thi].sdw[0] + mainMips.GPRs[ops.sec].sdw[0];
    }
    IvFujiSuperAsm(daddu) {
        mainMips.GPRs[ops.fir].dw[0] = mainMips.GPRs[ops.thi].dw[0] + mainMips.GPRs[ops.sec].dw[0];
    }
    IvFujiSuperAsm(dsub) {
        mainMips.GPRs[ops.fir].sdw[0] = mainMips.GPRs[ops.thi].sdw[0] - mainMips.GPRs[ops.sec].sdw[0];
    }
    IvFujiSuperAsm(dsubu) {
        mainMips.GPRs[ops.fir].dw[0] = mainMips.GPRs[ops.thi].dw[0] - mainMips.GPRs[ops.sec].dw[0];
    }
    IvFujiSuperAsm(srav) {
        // Shifting by a non immediate value (GPRs)
        i64* const shiftTo{mainMips.gprAt<i64>(ops.fir)};
        *shiftTo = mainMips.GPRs[ops.sec].swords[0] >>
            (mainMips.GPRs[ops.thi].sdw[0] & 0x1f);
    }
    IvFujiSuperAsm(ivXor) {
        mainMips.GPRs[ops.fir].dw[0] =
            (mainMips.GPRs[ops.thi].dw[0]) ^
            (mainMips.GPRs[ops.sec].dw[0]);
    }
    IvFujiSuperAsm(slt) {
        mainMips.GPRs[ops.fir].dw[0] =
            mainMips.GPRs[ops.thi].sdw[0] < mainMips.GPRs[ops.sec].sdw[0];
    }
    IvFujiSuperAsm(sll) {
        u8 shift{static_cast<u8>((ops.operation.inst >> 6) & 0x1f)};
        i64* const shiftTo{mainMips.gprAt<i64>(ops.fir)};
        *shiftTo = static_cast<i32>(mainMips.GPRs[ops.sec].words[0] << shift);
    }
    IvFujiSuperAsm(srl) {
        u8 right{static_cast<u8>((ops.operation.inst >> 6) & 0x1f)};
        i64* const shiftTo{mainMips.gprAt<i64>(ops.fir)};
        *shiftTo = static_cast<i32>(mainMips.GPRs[ops.sec].words[0] >> right);
    }
    IvFujiSuperAsm(sra) {
        i8 withBitSet{static_cast<i8>((ops.operation.inst >> 6) & 0x1f)};
        i64* const shiftTo{mainMips.gprAt<i64>(ops.fir)};
        *shiftTo = mainMips.GPRs[ops.sec].swords[0] >> withBitSet;
    }
    IvFujiSuperAsm(sllv) {
        // Shifting by a non immediate value (GPRs)
        i64* const shiftTo{mainMips.gprAt<i64>(ops.fir)};
        *shiftTo = static_cast<i32>(mainMips.GPRs[ops.sec].words[0] <<
            (mainMips.GPRs[ops.thi].bytes[0] & 0x1f));
    }
    IvFujiSuperAsm(srlv) {
        // Shifting by a non immediate value (GPRs)
        i64* const shiftTo{mainMips.gprAt<i64>(ops.fir)};
        *shiftTo = static_cast<i32>(mainMips.GPRs[ops.sec].words[0] >>
            (mainMips.GPRs[ops.thi].bytes[0] & 0x1f));
    }
}