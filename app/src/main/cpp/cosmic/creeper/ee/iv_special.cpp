#include <creeper/ee/cached_blocks.h>
#include <creeper/ee/fast_macros.h>
#include <engine/ee_core.h>
#include <console/backdoor.h>
#include <vm/emu_vm.h>

namespace cosmic::creeper::ee {
#define SPECIAL_IV_OP(name, op)\
    void MipsIvInterpreter::name(Operands ops) {\
        RD_SW = RS_SW op RT_SW;\
    }
#define SPECIAL_IV_OP_UNS(name, op)\
    void MipsIvInterpreter::name(Operands ops) {\
        RD_DW = RD_DW op RT_DW;\
    }

    SPECIAL_IV_OP(dadd, +)
    SPECIAL_IV_OP(dsub, -)

    SPECIAL_IV_OP_UNS(daddu, +)
    SPECIAL_IV_OP_UNS(dsubu, -)

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
        control->cause.exCode = 0x8;
        vm->dealWithSyscalls();
    }
}
