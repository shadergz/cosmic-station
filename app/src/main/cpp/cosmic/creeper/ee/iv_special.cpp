#include <creeper/ee/cached_blocks.h>
#include <engine/ee_core.h>
#include <console/backdoor.h>
#include <vm/emu_vm.h>

namespace cosmic::creeper::ee {
#define SPECIAL_IV_OP(op)\
        RD_SW = RS_SW op RT_SW
#define SPECIAL_IV_OP_UNS(op)\
        RD_DW = RD_DW op RT_DW

    void MipsIvInterpreter::dadd(Operands ops) {
        SPECIAL_IV_OP(+);
    }
    void MipsIvInterpreter::dsub(Operands ops) {
        SPECIAL_IV_OP(-);
    }
    void MipsIvInterpreter::daddu(Operands ops) {
        SPECIAL_IV_OP_UNS(+);
    }
    void MipsIvInterpreter::dsubu(Operands ops) {
        SPECIAL_IV_OP_UNS(-);
    }
    void MipsIvInterpreter::srav(Operands ops) {
        // Shifting by a non immediate value (GPRs)
        auto const address{cpu->gprAt<i64>(ops.rd)};
        *address = RT_WORDS_S >> (RS_SW & 0x1f);
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

        // https://github.com/PSI-Rockin/DobieStation/blob/68dd073e751960fd01c839ac34ce6e056d70024a/src/core/ee/emotion.cpp#L591
        // https://forums.pcsx2.net/Thread-Patch-Making-For-Dummies-SceMpegIsEnd
        // jr $ra = 0x03e00008;
        [[likely]] if (cpu->mipsRead<u32>(*cpu->eePc + 4) != 0x03e00008) {
            return;
        }
        // We haven't implemented MPEG decoders for now, so we have to skip all possible scenes
        const u32 next{cpu->mipsRead<u32>(*cpu->eePc)};
        // lw reg, 0x40($a0) = 0x8c800040;
        u32 code0{0x8c800040};
        // lw $v0, 0(reg) = ?;
        u32 code1{0x8c020000 | (next & 0x1f0000) << 5};

        if ((next & 0xffe0ffff) != code0)
            return;
        if (cpu->mipsRead<u32>(*cpu->eePc + 8) != code1)
            return;
        cpu->isABranch = {};
        cpu->delaySlot = {};
        cpu->GPRs[engine::$v0].qw = {};
    }
}
