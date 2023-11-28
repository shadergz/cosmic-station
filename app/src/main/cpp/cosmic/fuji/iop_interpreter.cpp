#include <fuji/iop_interpreter.h>
#include <console/backdoor.h>
#include <console/vm/emu_vm.h>
#include <common/global.h>
namespace cosmic::fuji {
    using namespace iop;
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
        if (opp == Slti) {
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
        raw_reference<console::vm::EmuVM> vm{redBox->openVm()};
        vm->dealWithSyscalls();
        redBox->leaveVm(vm);
    }

    u32 IOPInterpreter::execCopRow(u32 opcode, std::array<u8, 3> opeRegs) {
        u16 cop{static_cast<u16>((opcode >> 21) & 0x1f)};
        cop |= static_cast<u16>((opcode >> 26) & 0x3) << 8;
        switch (cop) {
        case CopMfc: mfc(Operands(opcode, opeRegs)); break;
        case CopMtc: mtc(Operands(opcode, opeRegs)); break;
        case CopRfe: rfe(Operands(opcode, opeRegs)); break;
        }
        return opcode;
    }
    u32 IOPInterpreter::execIO3S(u32 opcode, std::array<u8, 3> opeRegs) {
        switch (opcode & 0x3f) {
        case SpecialSyscall: syscall(Operands(opcode, opeRegs)); break;
        case SpecialMfhi: mfhi(Operands(opcode, opeRegs)); break;
        case SpecialMthi: mthi(Operands(opcode, opeRegs)); break;
        case SpecialOr: orSMips(Operands(opcode, opeRegs)); break;
        case SpecialXor: xorSMips(Operands(opcode, opeRegs)); break;
        case SpecialNor: nor(Operands(opcode, opeRegs)); break;
        }
        return opcode;
    }
    u32 IOPInterpreter::execIO3(u32 opcode, std::array<u8, 3> opeRegs) {
        switch (opcode >> 26) {
        case SpecialOp: return execIO3S(opcode, opeRegs);
        case 0x10 ... 0x13: return execCopRow(opcode, opeRegs);
        case Sltiu: sltBy(Operands(opcode, opeRegs)); break;
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
    thread_local std::array<char, 78> procedure;
    u32 IOPInterpreter::fetchPcInst() {
        u32 inst{ioMips.fetchByPC()};
        static const u32 pcPutc[]{0x00012c48, 0x0001420c, 0x0001430c};
        // Hooking all parameters of the putc function
        if (inst == pcPutc[0] || inst == pcPutc[1] || inst == pcPutc[2]) {
            u32 str{ioMips.IOGPRs[5]};
            const char* realStr{bit_cast<const char*>(ioMips.iopMem->iopUnalignedRead(str))};
            u64 textSize{ioMips.IOGPRs[6]};
            std::strncpy(procedure.data(), realStr,std::min(textSize, procedure.size()));

            userLog->info("IOP: putc function call intercepted, parameters {::#x} and {}, text {}", str, textSize, procedure.data());
        }
        [[unlikely]] if (ioMips.ioPc & 0x3) {
            userLog->error("IOP: Invalid PC value, issuing a interrupt of value 0x4");
            ioMips.handleException(0, 0x4);
            return static_cast<u32>(-1);
        }
        return inst;
    }
}
