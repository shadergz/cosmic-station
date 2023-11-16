#include <fuji/iop_interpreter.h>
#include <iop/iop_core.h>

#define SwOpcode(op)\
    op(static_cast<i32>(opcode), opeRegs[1], opeRegs[0]);\
    break
#define SwOpcodeSpecial(op)\
    op(static_cast<i32>(opcode), opeRegs[1], opeRegs[0], opeRegs[2]);\
    break

namespace zenith::fuji {
    IvFujiSpecialImpl(mfhi) {
        ioMips.IOGPRs[*gprSrc] = ioMips.hi;
    }
    IvFujiSpecialImpl(mthi) {
        ioMips.hi = ioMips.IOGPRs[*gprExt];
    }
    IvFuji3Impl(mfc) {
        if (((sfet >> 26) & 0x3) > 0)
            ;
        *gprDest = ioMips.cop.mfc(static_cast<u8>(*gprSrc));
    }
    IvFuji3Impl(mtc) {
        ioMips.cop.mtc(static_cast<u8>(*gprSrc), *gprDest);
    }
    IvFuji3Impl(rfe) {
        // ioMips.cop.rfe();
        ioMips.cop.status.kuc = ioMips.cop.status.kup;
        ioMips.cop.status.kup = ioMips.cop.status.kuo;

        ioMips.cop.status.iec = ioMips.cop.status.iep;
        ioMips.cop.status.iep = ioMips.cop.status.ieo;
    }
    IvFuji3Impl(sltBy) {
        gprSrc = &ioMips.IOGPRs[(sfet >> 16) & 0x1f];
        gprDest = &ioMips.IOGPRs[(sfet >> 21) & 0x1f];
        if ((sfet >> 26) == Slti) {
            i32 imm{sfet & 0xffff};
            *gprSrc = *gprSrc < imm;
        } else if ((sfet >> 26) == Sltiu) {
            u32 imm{static_cast<u32>(sfet & 0xffff)};
            *gprSrc = *gprSrc < imm;
        }
    }
    IvFujiSpecialImpl(orSMips) {
        *gprSrc = *gprDest | *gprExt;
    }
    IvFujiSpecialImpl(xorSMips) {
        *gprSrc = *gprDest ^ *gprExt;
    }
    IvFujiSpecialImpl(nor) {
        orSMips(sfet, gprDest, gprSrc, gprExt);
        *gprSrc = ~(*gprSrc);
    }
    u32 IOPInterpreter::execCopRow(u32 opcode, std::span<u32*> opeRegs) {
        u16 cop{static_cast<u16>((opcode >> 21) & 0x1f)};
        cop |= static_cast<u16>((opcode >> 26) & 0x3) << 8;
        switch (cop) {
        case CopMfc: SwOpcode(mfc);
        case CopMtc: SwOpcode(mtc);
        case CopRfe: SwOpcode(rfe);
        }
        return opcode;
    }
    u32 IOPInterpreter::execIO3S(u32 opcode, std::span<u32*> opeRegs) {
        u8 specialOp{static_cast<u8>(opcode & 0x3f)};
        switch (specialOp) {
        case SpecialMfhi: SwOpcodeSpecial(mfhi);
        case SpecialMthi: SwOpcodeSpecial(mthi);
        case SpecialOr:   SwOpcodeSpecial(orSMips);
        case SpecialXor:  SwOpcodeSpecial(xorSMips);
        }
        return opcode;
    }
    u32 IOPInterpreter::execIO3(u32 opcode, std::span<u32*> opeRegs) {
        switch (opcode) {
        case SpecialOp:
            execIO3S(opcode, opeRegs);
            break;
        case 0x10 ... 0x13:
            execCopRow(opcode, opeRegs);
            break;
        case Slti:
        case Sltiu:
            SwOpcode(sltBy);
        default:
            ;
        }
        return opcode;
    }
    u32 IOPInterpreter::executeCode() {
        u32 opcode{};
        std::array<u32*, 3> opeRegs;
        std::array<u8, 2> opes;
        do {
            opes[0] = (opcode >> 11) & 0x1f;
            opeRegs[0] = &ioMips.IOGPRs[opes[0]];
            opes[1] = (opcode >> 16) & 0x1f;
            opeRegs[1] = &ioMips.IOGPRs[opes[1]];
            opeRegs[2] = &ioMips.IOGPRs[(opcode >> 21) & 0x1f];

            opcode = fetchPcInst();
            execIO3(opcode, opeRegs);

            ioMips.cyclesToIO -= 4;

        } while (ioMips.cyclesToIO);
        return opcode;
    }
    u32 IOPInterpreter::fetchPcInst() {
        return ioMips.fetchByPC();
    }
}
