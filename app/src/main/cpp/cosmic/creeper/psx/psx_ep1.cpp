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
}
