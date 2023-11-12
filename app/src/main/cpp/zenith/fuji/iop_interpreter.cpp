#include <fuji/iop_interpreter.h>
#include <iop/iop_core.h>

namespace zenith::fuji {
    IvFuji3Impl(mfhi) {
        ioMips.IOGPRs[*gprSrc] = ioMips.hi;
    }
    IvFuji3Impl(mthi) {
        ioMips.hi = ioMips.IOGPRs[*gprSrc];
    }

    u32 IOPInterpreter::executeCode() {
        u32 opcode{};
        std::array<u32*, 2> opeRegs;
        std::array<u8, 2> opes;
        do {
            opes[0] = (opcode >> 11) & 0x1f;
            opeRegs[0] = &ioMips.IOGPRs[opes[0]];
            opes[1] = (opcode >> 21) & 0x1f;
            opeRegs[1] = &ioMips.IOGPRs[opes[1]];

            opcode = fetchPcInst();

#define SwOpcode(op)\
            op(static_cast<i32>(opcode), opeRegs[1], opeRegs[0]);\
            break

            switch (opcode) {
            case Mfhi: SwOpcode(mfhi);
            case Mthi: SwOpcode(mthi);
            }
            ioMips.cyclesToIO -= 4;

        } while (ioMips.cyclesToIO);
        return opcode;
    }

    u32 IOPInterpreter::fetchPcInst() {
        return ioMips.fetchByPC();
    }
}
