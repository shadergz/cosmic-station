#include <fuji/mipsiv_interpreter.h>
#include <eeiv/ee_engine.h>
#include <eeiv/ee_assembler.h>
#include <console/backdoor.h>
#include <console/emu_vm.h>
namespace cosmic::fuji {
    IvFujiSuperAsm(addi) {
        mainMips.GPRs[ops.sec].words[0] = ops.operation.pa16[0] +
            mainMips.GPRs[ops.thi].words[0];
    }
    IvFujiSuperAsm(slti) {
        u8 cmp{mainMips.GPRs[ops.thi].hw[0] < (ops.operation.inst & 0xffff)};
        mainMips.GPRs[ops.sec].dw[0] = cmp;
    }
    IvFujiSuperAsm(sw) {
        // The 16-bit signed offset is added to the contents of GPR base to form the effective address
        u32 stAddr{mainMips.GPRs[ops.thi].words[0] + ops.operation.inst & 0xffff};
        mainMips.directWrite(stAddr, mainMips.GPRs[ops.sec].words[0]);
    }
    // if (cond < {0, null}) ...
    IvFujiSuperAsm(bltzal) {
        // With the 18-bit signed instruction offset, the conditional branch range is ± 128 KBytes
        i32 jump{static_cast<i32>(ops.operation.pa16[0] << 2)};
        // GPR[31] ← PC + 8
        *mainMips.gprAt<u32>(eeiv::$ra) = *mainMips.lastPC + 8;
        mainMips.branchByCondition(mainMips.GPRs[ops.thi].dw[0] < 0, jump);
    }
    IvFujiSuperAsm(bgez) {
        i32 br{(ops.operation.sins & 0xffff) << 2};
        mainMips.branchByCondition(mainMips.GPRs[ops.thi].dw[0] >= 0, br);
    }
    IvFujiSuperAsm(bgezl) {
        const i32 br{(ops.operation.sins & 0xffff) << 2};
        mainMips.branchOnLikely(mainMips.GPRs[ops.thi].dw[0] >= 0, br);
    }
    IvFujiSuperAsm(bgezall) {
        // Place the return address link in GPR 31
        mainMips.GPRs[eeiv::$ra].words[0] = *mainMips.eePC + 8;
        u8 cmp{mainMips.GPRs[ops.thi].dw[0] >= 0};
        u16 imm{static_cast<u16>((ops.operation.sins & 0xffff) << 2)};
        mainMips.branchOnLikely(cmp, imm);
    }
    IvFujiSuperAsm(mtsab) {
        u32 sabbath{mainMips.GPRs[ops.thi].words[0]};
        u16 black{static_cast<u16>(ops.operation.sins & 0xffff)};
        sabbath = (sabbath & 0xf) ^ (black & 0xf);
        mainMips.sa = sabbath;
    }
    IvFujiSuperAsm(mtsah) {
        u16 imm{static_cast<u16>(ops.operation.pa16[0])};
        const u32 value{(mainMips.GPRs[ops.thi].words[0] & 0x7) ^ (imm & 0x7)};
        mainMips.sa = value * 2;
    }

#define CalcOffset(reg)\
    static_cast<u32>(ops.operation.ps16[0] + mainMips.GPRs[reg].swords[0])

#define EffectiveLoadRegs(reg, offset, from)\
    mainMips.GPRs[reg].dw[0] =\
        static_cast<u64>(mainMips.tableRead<from>(offset))
#define SignedEffectiveLoadRegs(reg, offset, from)\
    *reinterpret_cast<i64*>(mainMips.GPRs[reg].dw[0]) =\
        static_cast<i64>(mainMips.tableRead<from>(offset))

    IvFujiSuperAsm(lb) {
        SignedEffectiveLoadRegs(ops.fir, CalcOffset(ops.sec), i32);
    }
    IvFujiSuperAsm(lh) {
        SignedEffectiveLoadRegs(ops.fir, CalcOffset(ops.sec), u16);
    }
    IvFujiSuperAsm(lw) {
        SignedEffectiveLoadRegs(ops.fir, CalcOffset(ops.sec), u32);
    }
    IvFujiSuperAsm(lbu) {
        EffectiveLoadRegs(ops.fir, CalcOffset(ops.sec), u32);
    }
    IvFujiSuperAsm(lhu) {
        EffectiveLoadRegs(ops.fir, CalcOffset(ops.sec), u16);
    }
    IvFujiSuperAsm(lwu) {
        EffectiveLoadRegs(ops.fir, CalcOffset(ops.sec), u32);
    }

    IvFujiSuperAsm(ld) {
        EffectiveLoadRegs(ops.fir, CalcOffset(ops.sec), u64);
    }
    IvFujiSuperAsm(sd) {
        mainMips.directWrite(CalcOffset(ops.sec), mainMips.GPRs[ops.fir].dw[0]);
    }
#undef LoadBy
    IvFujiSuperAsm(cache) {
        const i32 as{mainMips.GPRs[ops.sec].swords[0] + ops.operation.ps16[0]};
        switch (ops.operation.pa8[3]) {
        case 0x07:
            mainMips.cop0.invIndexed(static_cast<u32>(as));
            break;
        }
    }
    IvFujiSuperAsm(nop) {}
    IvFujiSuperAsm(ivXor) {
        mainMips.GPRs[ops.fir].dw[0] =
            (mainMips.GPRs[ops.thi].dw[0]) ^
            (mainMips.GPRs[ops.sec].dw[0]);
    }
    IvFujiSuperAsm(slt) {
        mainMips.GPRs[ops.fir].dw[0] =
            mainMips.GPRs[ops.thi].sdw[0] < mainMips.GPRs[ops.sec].sdw[0];
    }
    IvFujiSuperAsm(iBreak) {
        mainMips.handleException(1, 0x80000180, 0x9);
    }
    IvFujiSuperAsm(syscall) {
        mainMips.cop0.cause.exCode = 0x8;
        // We need to directly handle these syscall, instead of mainMips.chPC(0x80000180);
        auto vm{redBox->openVm()};
        vm->dealWithSyscalls();
        redBox->leaveVm(vm);
    }
}
