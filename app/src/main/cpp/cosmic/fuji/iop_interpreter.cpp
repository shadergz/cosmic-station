#include <fuji/iop_interpreter.h>
#include <console/backdoor.h>
#include <console/emu_vm.h>

#define SwOpcode(op)\
    op(Operands(opcode, opeRegs));\
    break

namespace cosmic::fuji {
    IvFujiIopAsm(mfhi) {
        u32 target{ioMips.IOGPRs[ops.fir]};
        ioMips.IOGPRs[target] = ioMips.hi;
    }
    IvFujiIopAsm(mthi) {
        ioMips.hi = ioMips.IOGPRs[ioMips.IOGPRs[ops.thi]];
    }
    IvFujiIopAsm(mfc) {
        if (((ops.operation.pa8[3]) & 0x3) > 0)
            ;
        u8 cor{static_cast<u8>(ioMips.IOGPRs[ops.thi])};
        u32 fetched{ioMips.cop.mfc(cor)};
        ioMips.IOGPRs[ops.sec] = fetched;
    }
    IvFujiIopAsm(mtc) {
        std::array<u32, 2> mtcOps;

        mtcOps[0] = ioMips.IOGPRs[ops.fir];
        mtcOps[1] = ioMips.IOGPRs[ops.fir];
        ioMips.cop.mtc(static_cast<u8>(mtcOps[0]), mtcOps[1]);
    }
    IvFujiIopAsm(rfe) {
        // ioMips.cop.rfe();
        ioMips.cop.status.kuc = ioMips.cop.status.kup;
        ioMips.cop.status.kup = ioMips.cop.status.kuo;

        ioMips.cop.status.iec = ioMips.cop.status.iep;
        ioMips.cop.status.iep = ioMips.cop.status.ieo;
    }
    IvFujiIopAsm(sltBy) {
        u32* gprSrc = &ioMips.IOGPRs[ops.thi];
        u32* gprDest = &ioMips.IOGPRs[ops.sec];
        u8 opp{ops.operation.pa8[3]};
        if (opp == IOPOpcodes::Slti) {
            i32 imm{ops.operation.sins & 0xffff};
            *gprDest = *gprSrc < imm;
        } else if (opp == Sltiu) {
            u32 imm{ops.operation.inst & 0xffff};
            *gprDest = *gprSrc < imm;
        }
    }
    IvFujiIopAsm(orSMips) {
        ioMips.IOGPRs[ops.fir] = ioMips.IOGPRs[ops.thi] | ioMips.IOGPRs[ops.sec];
    }
    IvFujiIopAsm(xorSMips) {
        ioMips.IOGPRs[ops.fir] = ioMips.IOGPRs[ops.thi] ^ ioMips.IOGPRs[ops.sec];
    }
    IvFujiIopAsm(nor) {
        orSMips(ops);
        ioMips.IOGPRs[ops.fir] = ~ioMips.IOGPRs[ops.fir];
    }
    IvFujiIopAsm(syscall) {
        ioMips.cop.cause.code = 0x8;
        raw_reference<console::EmuVM> vm{redBox->openVm()};
        vm->dealWithSyscalls();
        redBox->leaveVm(vm);
    }

    u32 IOPInterpreter::execCopRow(u32 opcode, std::array<u8, 3> opeRegs) {
        u16 cop{static_cast<u16>((opcode >> 21) & 0x1f)};
        cop |= static_cast<u16>((opcode >> 26) & 0x3) << 8;
        switch (cop) {
        case CopMfc: SwOpcode(mfc);
        case CopMtc: SwOpcode(mtc);
        case CopRfe: SwOpcode(rfe);
        }
        return opcode;
    }
    u32 IOPInterpreter::execIO3S(u32 opcode, std::array<u8, 3> opeRegs) {
        switch (opcode & 0x3f) {
        case SpecialSyscall: SwOpcode(syscall);
        case SpecialMfhi: SwOpcode(mfhi);
        case SpecialMthi: SwOpcode(mthi);
        case SpecialOr: SwOpcode(orSMips);
        case SpecialXor: SwOpcode(xorSMips);
        case SpecialNor: SwOpcode(nor);
        }
        return opcode;
    }
    u32 IOPInterpreter::execIO3(u32 opcode, std::array<u8, 3> opeRegs) {
        switch (opcode >> 26) {
        case SpecialOp:
            execIO3S(opcode, opeRegs);
            break;
        case 0x10 ... 0x13:
            execCopRow(opcode, opeRegs);
            break;
        case Sltiu: SwOpcode(sltBy);
        default:
            ;
        }
        return opcode;
    }
    u32 IOPInterpreter::executeCode() {
        u32 opcode{};
        std::array<u8, 3> opes;
        do {
            opcode = fetchPcInst();

            opes[0] = (opcode >> 11) & 0x1f;
            opes[1] = (opcode >> 16) & 0x1f;
            opes[2] = (opcode >> 21) & 0x1f;

            execIO3(opcode, opes);
            ioMips.cyclesToIO -= 4;

        } while (ioMips.cyclesToIO);
        return opcode;
    }
    u32 IOPInterpreter::fetchPcInst() {
        return ioMips.fetchByPC();
    }
}
