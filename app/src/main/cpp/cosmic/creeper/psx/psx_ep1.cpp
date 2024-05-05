#include <creeper/psx/iop_interpreter.h>
#include <iop/iop_core.h>

namespace cosmic::creeper::psx {
    void IopInterpreter::orMips(Operands ops) {
        cpu->ioGPRs[ops.rd] = cpu->ioGPRs[ops.rs] | cpu->ioGPRs[ops.rt];
    }
    void IopInterpreter::xorMips(Operands ops) {
        cpu->ioGPRs[ops.rd] = cpu->ioGPRs[ops.rs] ^ cpu->ioGPRs[ops.rt];
    }

    void IopInterpreter::nor(Operands ops) {
        orMips(ops);
        cpu->ioGPRs[ops.rd] = ~cpu->ioGPRs[ops.rd];
    }
    void IopInterpreter::lui(Operands ops) {
        cpu->ioGPRs[ops.rt] = (ops.inst & 0xffff) << 16;
    }
    void IopInterpreter::ori(Operands ops) {
        cpu->ioGPRs[ops.rt] = cpu->ioGPRs[ops.rs] | (ops.inst & 0xffff);
    }
    void IopInterpreter::lw(Operands ops) {
        u32 effAddr{cpu->ioGPRs[ops.rs] + (ops.sins & 0xffff)};
        if (effAddr & 1) {
        }
        cpu->ioGPRs[ops.rt] = static_cast<u32>(cpu->iopRead<i32>(effAddr));
    }
    void IopInterpreter::andi(Operands ops) {
        cpu->ioGPRs[ops.rt] = cpu->ioGPRs[ops.rs] & (ops.sins & 0xffff);
    }
    void IopInterpreter::addi(Operands ops) {
        cpu->ioGPRs[ops.rt] = cpu->ioGPRs[ops.rs] + (ops.sins & 0xffff);
    }
    void IopInterpreter::addiu(Operands ops) {
        cpu->ioGPRs[ops.rt] = cpu->ioGPRs[ops.rs] + (ops.inst & 0xffff);
    }
    void IopInterpreter::sw(Operands ops) {
        u32 effective{cpu->ioGPRs[ops.rd] + (ops.sins & 0xffff)};
        cpu->iopWrite<u32>(effective, cpu->ioGPRs[ops.rt]);
    }
}
