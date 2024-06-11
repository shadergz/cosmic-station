#include <creeper/cached_blocks.h>
#include <ee/ee_core.h>
#include <console/backdoor.h>

#include <vm/emu_vm.h>
namespace cosmic::creeper {
    void MipsIvInterpreter::mult(Operands ops) {
        auto result{signedDoReg(ops.rs) * signedDoReg(ops.rt)};

        cpu->setLoHi(static_cast<u64>(result));
        signedDo64Reg(ops.rd) = cpu->mulDivStorage[0];
    }
    void MipsIvInterpreter::multu(Operands ops) {
        auto multi{doReg(ops.rs) * doReg(ops.rt)};

        cpu->setLoHi(multi);
        do64Reg(ops.rd) = static_cast<u64>(cpu->mulDivStorage[0]);
    }
    void MipsIvInterpreter::div(Operands ops) {
        const i32 dividend{signedDoReg(ops.rs)};
        const i32 divisor{signedDoReg(ops.rt)};

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
        auto const dividend{signedDoReg(ops.rs)};
        auto const divisor{signedDoReg(ops.rt)};

        if (divisor) {
            cpu->setLoHi(dividend / divisor, dividend % divisor);
        } else {
            cpu->setLoHi(-1, dividend);
        }
    }

    void MipsIvInterpreter::add(Operands ops) {
        doReg(ops.rd) = doReg(ops.rs) + doReg(ops.rt);
    }
    void MipsIvInterpreter::addu(Operands ops) {
        signedDoReg(ops.rd) = signedDoReg(ops.rs) + signedDoReg(ops.rt);
    }
    void MipsIvInterpreter::sub(Operands ops) {
        doReg(ops.rd) = doReg(ops.rs) - doReg(ops.rt);
    }
    void MipsIvInterpreter::subu(Operands ops) {
        signedDoReg(ops.rd) = signedDoReg(ops.rs) - signedDoReg(ops.rt);
    }

    void MipsIvInterpreter::ori(Operands ops) {
        do64Reg(ops.rt) = do64Reg(ops.rs) | getOffset(ops);
    }
    void MipsIvInterpreter::xori(Operands ops) {
        do64Reg(ops.rt) = do64Reg(ops.rs) ^ getOffset(ops);
    }
    void MipsIvInterpreter::slt(Operands ops) {
        do64Reg(ops.rd) = signedDo64Reg(ops.rs) <
            signedDo64Reg(ops.rt);
    }

    void MipsIvInterpreter::sll(Operands ops) {
        if (ops.rt) {
            auto& address{cpu->GPRs[ops.rd].sdw[0]};
            address = signedDoReg(ops.rt) << (static_cast<u8>(ops.inst >> 6) & 0x1f);
        }
    }
    void MipsIvInterpreter::srl(Operands ops) {
        if (ops.rt) {
            auto& address{cpu->GPRs[ops.rd].sdw[0]};
            address = signedDoReg(ops.rt) >> (static_cast<u8>(ops.inst >> 6) & 0x1f);
        }
    }

    void MipsIvInterpreter::sra(Operands ops) {
        auto withBitSet{static_cast<i8>((ops.inst >> 6) & 0x1f)};
        auto& address{cpu->GPRs[ops.rd].sdw[0]};

        address = signedDoReg(ops.rt) >> withBitSet;
    }
    void MipsIvInterpreter::sllv(Operands ops) {
        // Shifting by a non immediate value (GPRs)
        auto& address{cpu->GPRs[ops.rd].dw[0]};

        address = doReg(ops.rt) << (cpu->GPRs[ops.rs].uh[0] & 0x1f);
    }
    void MipsIvInterpreter::srlv(Operands ops) {
        // Shifting by a non immediate value (GPRs)
        auto& address{cpu->GPRs[ops.rd].sdw[0]};
        address = doReg(ops.rt) >> (cpu->GPRs[ops.rs].uh[0] & 0x1f);
    }

    void MipsIvInterpreter::dadd(Operands ops) {
        do64Reg(ops.rd) = do64Reg(ops.rs) + do64Reg(ops.rt);
    }
    void MipsIvInterpreter::dsub(Operands ops) {
        do64Reg(ops.rd) = do64Reg(ops.rs) - do64Reg(ops.rt);
    }
    void MipsIvInterpreter::daddu(Operands ops) {
        do64Reg(ops.rd) = do64Reg(ops.rs) + do64Reg(ops.rt);
    }
    void MipsIvInterpreter::dsubu(Operands ops) {
        do64Reg(ops.rd) = do64Reg(ops.rs) - do64Reg(ops.rt);
    }

    void MipsIvInterpreter::iAnd(Operands ops) {
        doReg(ops.rd) = doReg(ops.rs) & doReg(ops.rt);
    }
    void MipsIvInterpreter::iOr(Operands ops) {
        doReg(ops.rd) = doReg(ops.rs) | doReg(ops.rt);
    }
    void MipsIvInterpreter::iXor(Operands ops) {
        doReg(ops.rd) = doReg(ops.rs) ^ doReg(ops.rt);
    }
    void MipsIvInterpreter::nor(Operands ops) {
        doReg(ops.rd) = ~(doReg(ops.rs) | doReg(ops.rt));
    }

    void MipsIvInterpreter::srav(Operands ops) {
        // Shifting by a non immediate value (GPRs)
        auto& address{cpu->GPRs[ops.rd].sdw[0]};
        address = signedDoReg(ops.rt) >> (doReg(ops.rs) & 0x1f);
    }
    void MipsIvInterpreter::iBreak([[maybe_unused]] Operands ops) {
        cpu->handleException(1, 0x80000180, 0x9);
    }
    void MipsIvInterpreter::iSyscall(Operands ops) {
        // We need to directly handle these syscall, instead of cpu.chPc(0x80000180)
        c0->cause.exCode = 0x8;

        vm->dealWithSyscalls();
    }
    void MipsIvInterpreter::jr(Operands ops) {
        cpu->isABranch = true;
        const auto jumpAddress{cpu->GPRs[ops.rs].words[0]};
        cpu->chPc(jumpAddress);
        cpu->delaySlot = 1;

        // https://github.com/PSI-Rockin/DobieStation/blob/master/src/core/ee/emotion.cpp#L591
        // https://forums.pcsx2.net/Thread-Patch-Making-For-Dummies-SceMpegIsEnd
        // jr $ra = 0x03e00008;
        [[likely]] if (cpu->mipsRead<u32>(cpu->eePc + 4) != 0x03e00008) {
            return;
        }
        // We haven't implemented MPEG decoders for now, so we have to skip all possible scenes
        const u32 next{cpu->mipsRead<u32>(cpu->eePc)};
        // lw reg, 0x40($a0) = 0x8c800040;
        u32 code0{0x8c800040};
        // lw $v0, 0(reg) = ?;
        u32 code1{0x8c020000 | (next & 0x1f0000) << 5};

        if ((next & 0xffe0ffff) != code0)
            return;
        if (cpu->mipsRead<u32>(cpu->eePc + 8) != code1)
            return;
        cpu->isABranch = {};
        cpu->delaySlot = {};
        cpu->GPRs[ee::$v0].qw = {1};
    }
    void MipsIvInterpreter::addi(Operands ops) {
        doReg(ops.rt) = getOffset(ops) + doReg(ops.rs);
    }
    void MipsIvInterpreter::addiu(Operands ops) {
        doReg(ops.rt) = getOffset(ops) + doReg(ops.rs);
    }
    void MipsIvInterpreter::lui(Operands ops) {
        do64Reg(ops.rt) = static_cast<u64>(signedGetOffset(ops) << 16);
    }

    void MipsIvInterpreter::slti(Operands ops) {
        do64Reg(ops.rt) = (doReg(ops.rs) & 0xffff) < signedGetOffset(ops);
    }

    void MipsIvInterpreter::sw(Operands ops) {
        // The 16-bit signed offset is added to the contents of GPR base to form the effective address
        auto stAddr{static_cast<i32>(doReg(ops.base)) + signedGetOffset(ops)};
        cpu->mipsWrite(static_cast<u32>(stAddr), doReg(ops.rt));
    }

    // if (cond < {0, null}) ...
    void MipsIvInterpreter::bltzal(Operands ops) {
        // With the 18-bit signed instruction offset, the conditional branch range is ± 128 KBytes
        // GPR[31] ← PC + 8
        auto jump{static_cast<i32>(ops.pa16[0] << 2)};

        cpu->GPRs[ee::$ra].sdw[0] = cpu->lastPc + 8;
        cpu->branchByCondition(signedDo64Reg(ops.rs) < 0, jump);
    }
    void MipsIvInterpreter::bne(Operands ops) {
        cpu->branchByCondition(
            do64Reg(ops.rs) != do64Reg(ops.rt),
            signedGetOffset(ops) << 2);
    }
    void MipsIvInterpreter::bgez(Operands ops) {
        cpu->branchByCondition(signedDo64Reg(ops.rs) >= 0, signedGetOffset(ops) << 2);
    }
    void MipsIvInterpreter::bgezl(Operands ops) {
        cpu->branchOnLikely(signedDo64Reg(ops.rs) >= 0, signedGetOffset(ops) << 2);
    }
    void MipsIvInterpreter::bgezall(Operands ops) {
        // Place the return address link in GPR 31
        doReg(ee::$ra) = cpu->eePc + 8;
        cpu->branchOnLikely(signedDo64Reg(ops.rs) >= 0,
            static_cast<i32>(getOffset(ops) << 2));
    }
    void MipsIvInterpreter::mtsab(Operands ops) {
        cpu->sa = (doReg(ops.rs) & 0xf) ^
            (signedGetOffset(ops) & 0xf);

    }
    void MipsIvInterpreter::mtsah(Operands ops) {
        auto immediate{static_cast<u16>(ops.pa16[0])};
        const u32 value{(doReg(ops.rs) & 0x7) ^ (immediate & 0x7)};
        cpu->sa = value * 2;
    }

    const auto MipsIvInterpreter::calcOffset(Operands ops, auto reg) {
        return static_cast<u32>(ops.ps16[0] + signedDoReg(reg));
    }

    void MipsIvInterpreter::lb(Operands ops) {
        signedDoReg(ops.rt) = cpu->mipsRead<i8>(calcOffset(ops, ops.base));
    }
    void MipsIvInterpreter::lh(Operands ops) {
        signedDoReg(ops.rt) = cpu->mipsRead<i16>(calcOffset(ops, ops.base));
    }
    void MipsIvInterpreter::lw(Operands ops) {
        signedDoReg(ops.rt) = cpu->mipsRead<i32>(calcOffset(ops, ops.base));
    }
    void MipsIvInterpreter::lbu(Operands ops) {
        doReg(ops.rt) = cpu->mipsRead<u8>(calcOffset(ops, ops.base));
    }
    void MipsIvInterpreter::lhu(Operands ops) {
        doReg(ops.rt) = cpu->mipsRead<u16>(calcOffset(ops, ops.base));
    }
    void MipsIvInterpreter::lwu(Operands ops) {
        doReg(ops.rt) = cpu->mipsRead<u32>(calcOffset(ops, ops.base));
    }

    void MipsIvInterpreter::ld(Operands ops) {
        do64Reg(ops.rt) = cpu->mipsRead<u64>(calcOffset(ops, ops.base));
    }
    void MipsIvInterpreter::sd(Operands ops) {
        cpu->mipsWrite(calcOffset(ops, ops.base), do64Reg(ops.rt));
    }
    void MipsIvInterpreter::cache(Operands ops) {
        switch (ops.pa8[3]) {
        case 0x7:
            c0->invIndexed(static_cast<u32>(signedDoReg(ops.rs) + ops.ps16[0]));
        }
    }
    void MipsIvInterpreter::nop(Operands ops) {}
    // If the value in GPR rt is equal to zero, then the contents of GPR rs are placed into GPR rd
    // The zero value tested here is the “condition false” result from the SLT, SLTI, SLTU, and
    // SLTIU comparison instructions
    void MipsIvInterpreter::movz(Operands ops) {
        if (!do64Reg(ops.rt))
            do64Reg(ops.rd) = do64Reg(ops.rs);
    }
    void MipsIvInterpreter::movn(Operands ops) {
        if (!ops.rd)
            return;
        if (do64Reg(ops.rt))
            do64Reg(ops.rd) = do64Reg(ops.rs);
    }
}