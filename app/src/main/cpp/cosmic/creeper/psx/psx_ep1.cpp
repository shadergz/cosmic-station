#include <creeper/psx/iop_interpreter.h>
#include <iop/iop_core.h>

namespace cosmic::creeper::psx {
    void IopInterpreter::orMips(Operands ops) {
        ioMips->ioGPRs[ops.rd] = ioMips->ioGPRs[ops.rs] | ioMips->ioGPRs[ops.rt];
    }
    void IopInterpreter::xorMips(Operands ops) {
        ioMips->ioGPRs[ops.rd] = ioMips->ioGPRs[ops.rs] ^ ioMips->ioGPRs[ops.rt];
    }

    void IopInterpreter::nor(Operands ops) {
        orMips(ops);
        ioMips->ioGPRs[ops.rd] = ~ioMips->ioGPRs[ops.rd];
    }
    void IopInterpreter::lui(Operands ops) {
        ioMips->ioGPRs[ops.rt] = (ops.inst & 0xffff) << 16;
    }
    void IopInterpreter::ori(Operands ops) {
        ioMips->ioGPRs[ops.rt] = ioMips->ioGPRs[ops.rs] | (ops.inst & 0xffff);
    }
    void IopInterpreter::lw(Operands ops) {
        u32 effAddr{ioMips->ioGPRs[ops.rs] + (ops.sins & 0xffff)};
        if (effAddr & 1)
            ;
        ioMips->ioGPRs[ops.rt] = static_cast<u32>(ioMips->iopRead<i32>(effAddr));
    }
    void IopInterpreter::andi(Operands ops) {
        ioMips->ioGPRs[ops.rt] = ioMips->ioGPRs[ops.rs] & (ops.sins & 0xffff);
    }
    void IopInterpreter::addi(Operands ops) {
        ioMips->ioGPRs[ops.rt] = ioMips->ioGPRs[ops.rt] + (ops.sins & 0xffff);
    }
    void IopInterpreter::addiu(Operands ops) {
        ioMips->ioGPRs[ops.rt] = ioMips->ioGPRs[ops.rs] + (ops.inst & 0xffff);
    }
    void IopInterpreter::sw(Operands ops) {
        u32 effective{ioMips->ioGPRs[ops.rd] + (ops.sins & 0xffff)};
        ioMips->iopWrite<u32>(effective, ioMips->ioGPRs[ops.rt]);
    }
}
