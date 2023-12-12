#include <fuji/mipsiv_interpreter.h>
#include <engine/ee_core.h>
#include <engine/ee_assembler.h>
#include <console/backdoor.h>
#include <console/vm/emu_vm.h>
namespace cosmic::fuji {
    IV_FUJI_SUPER_ASM(addi) {
        mainMips.GPRs[ops.sec].words[0] = ops.operation.pa16[0] +
            mainMips.GPRs[ops.thi].words[0];
    }
    IV_FUJI_SUPER_ASM(slti) {
        u8 cmp{mainMips.GPRs[ops.thi].hw[0] < (ops.operation.sins & 0xffff)};
        mainMips.GPRs[ops.sec].dw[0] = cmp;
    }
    IV_FUJI_SUPER_ASM(sw) {
        // The 16-bit signed offset is added to the contents of GPR base to form the effective address
        u32 stAddr{mainMips.GPRs[ops.thi].words[0] + ops.operation.inst & 0xffff};
        mainMips.mipsWrite(stAddr, mainMips.GPRs[ops.sec].words[0]);
    }
    // if (cond < {0, null}) ...
    IV_FUJI_SUPER_ASM(bltzal) {
        // With the 18-bit signed instruction offset, the conditional branch range is ± 128 KBytes
        i32 jump{static_cast<i32>(ops.operation.pa16[0] << 2)};
        // GPR[31] ← PC + 8
        *mainMips.gprAt<u32>(engine::$ra) = *mainMips.lastPC + 8;
        mainMips.branchByCondition(mainMips.GPRs[ops.thi].dw[0] < 0, jump);
    }
    IV_FUJI_SUPER_ASM(bne) {
        const u64 cond1{mainMips.GPRs[ops.thi].dw[0]};
        const u64 cond2{mainMips.GPRs[ops.sec].dw[0]};
        mainMips.branchByCondition(cond1 != cond2, (ops.operation.sins & 0xffff) << 2);
    }
    IV_FUJI_SUPER_ASM(bgez) {
        i32 br{(ops.operation.sins & 0xffff) << 2};
        mainMips.branchByCondition(mainMips.GPRs[ops.thi].dw[0] >= 0, br);
    }
    IV_FUJI_SUPER_ASM(bgezl) {
        const i32 br{(ops.operation.sins & 0xffff) << 2};
        mainMips.branchOnLikely(mainMips.GPRs[ops.thi].dw[0] >= 0, br);
    }
    IV_FUJI_SUPER_ASM(bgezall) {
        // Place the return address link in GPR 31
        mainMips.GPRs[engine::$ra].words[0] = *mainMips.eePC + 8;
        u8 cmp{mainMips.GPRs[ops.thi].dw[0] >= 0};
        u16 imm{static_cast<u16>((ops.operation.sins & 0xffff) << 2)};
        mainMips.branchOnLikely(cmp, imm);
    }
    IV_FUJI_SUPER_ASM(mtsab) {
        u32 sabbath{mainMips.GPRs[ops.thi].words[0]};
        u16 black{static_cast<u16>(ops.operation.sins & 0xffff)};
        sabbath = (sabbath & 0xf) ^ (black & 0xf);
        mainMips.sa = sabbath;
    }
    IV_FUJI_SUPER_ASM(mtsah) {
        u16 imm{static_cast<u16>(ops.operation.pa16[0])};
        const u32 value{(mainMips.GPRs[ops.thi].words[0] & 0x7) ^ (imm & 0x7)};
        mainMips.sa = value * 2;
    }

#define CALC_OFFSET(reg)\
    static_cast<u32>(ops.operation.ps16[0] + mainMips.GPRs[reg].swords[0])

#define EFFECTIVE_LOAD_REGS(reg, offset, from)\
    mainMips.GPRs[reg].dw[0] =\
        static_cast<u64>(mainMips.mipsRead<from>(offset))
#define SIGNED_EFFECTIVE_LOAD_REGS(reg, offset, from)\
    *reinterpret_cast<i64*>(mainMips.GPRs[reg].dw[0]) =\
        static_cast<i64>(mainMips.mipsRead<from>(offset))

    IV_FUJI_SUPER_ASM(lb) {
        SIGNED_EFFECTIVE_LOAD_REGS(ops.fir, CALC_OFFSET(ops.sec), i32);
    }
    IV_FUJI_SUPER_ASM(lh) {
        SIGNED_EFFECTIVE_LOAD_REGS(ops.fir, CALC_OFFSET(ops.sec), u16);
    }
    IV_FUJI_SUPER_ASM(lw) {
        SIGNED_EFFECTIVE_LOAD_REGS(ops.fir, CALC_OFFSET(ops.sec), u32);
    }
    IV_FUJI_SUPER_ASM(lbu) {
        EFFECTIVE_LOAD_REGS(ops.fir, CALC_OFFSET(ops.sec), u32);
    }
    IV_FUJI_SUPER_ASM(lhu) {
        EFFECTIVE_LOAD_REGS(ops.fir, CALC_OFFSET(ops.sec), u16);
    }
    IV_FUJI_SUPER_ASM(lwu) {
        EFFECTIVE_LOAD_REGS(ops.fir, CALC_OFFSET(ops.sec), u32);
    }

    IV_FUJI_SUPER_ASM(ld) {
        EFFECTIVE_LOAD_REGS(ops.fir, CALC_OFFSET(ops.sec), u64);
    }
    IV_FUJI_SUPER_ASM(sd) {
        mainMips.mipsWrite(CALC_OFFSET(ops.sec), mainMips.GPRs[ops.fir].dw[0]);
    }
    IV_FUJI_SUPER_ASM(cache) {
        const i32 as{mainMips.GPRs[ops.sec].swords[0] + ops.operation.ps16[0]};
        switch (ops.operation.pa8[3]) {
        case 0x07:
            mainMips.ctrl0.invIndexed(static_cast<u32>(as));
            break;
        }
    }
    IV_FUJI_SUPER_ASM(nop) {}
    // If the value in GPR rt is equal to zero, then the contents of GPR rs are placed into GPR rd
    // The zero value tested here is the “condition false” result from the SLT, SLTI, SLTU, and
    // SLTIU comparison instructions
    IV_FUJI_SUPER_ASM(movz) {
        const u64 attr{mainMips.GPRs[ops.thi].dw[0]};
        if (!mainMips.GPRs[ops.sec].dw[0])
            mainMips.GPRs[ops.fir].dw[0] = attr;
    }
    IV_FUJI_SUPER_ASM(movn) {
        if (mainMips.GPRs[ops.sec].dw[0])
            mainMips.GPRs[ops.fir].dw[0] = mainMips.GPRs[ops.thi].dw[0];
    }

    IV_FUJI_SUPER_ASM(ivBreak) {
        mainMips.handleException(1, 0x80000180, 0x9);
    }
    IV_FUJI_SUPER_ASM(ivSyscall) {
        mainMips.ctrl0.cause.exCode = 0x8;
        // We need to directly handle these syscall, instead of mainMips.chPC(0x80000180);
        auto vm{redBox->openVm()};
        vm->dealWithSyscalls();
        redBox->leaveVm(vm);
    }
}
