#include <range/v3/algorithm.hpp>
#include <console/backdoor.h>
#include <common/global.h>
#include <translator/psx/iop_interpreter.h>
#include <vm/emu_vm.h>

namespace cosmic::translator::psx {
    using namespace iop;
    void IopInterpreter::bne(Operands ops) {
        ioMips->takeBranchIf(ioMips->IoGPRs[ops.thi] != ioMips->IoGPRs[ops.sec],
            (ops.operation.sins & 0xffff) << 2);
    }
    void IopInterpreter::blez(Operands ops) {
        ioMips->takeBranchIf(ioMips->IoGPRs[ops.thi] <= 0, (ops.operation.sins & 0xffff) << 2);
    }
    void IopInterpreter::mfhi(Operands ops) {
        u32 target{ioMips->IoGPRs[ops.fir]};
        ioMips->IoGPRs[target] = ioMips->hi;
    }
    void IopInterpreter::mthi(Operands ops) {
        ioMips->hi = ioMips->IoGPRs[ioMips->IoGPRs[ops.thi]];
    }

    void IopInterpreter::mfc(Operands ops) {
        if (((ops.operation.pa8[3]) & 0x3) > 0)
            ;
        u32 fetched{ioMips->cop.mfc(ops.fir)};
        ioMips->IoGPRs[ops.sec] = fetched;
    }

    void IopInterpreter::mtc(Operands ops) {
        std::array<u32, 2> mtcOps;

        mtcOps[0] = ioMips->IoGPRs[ops.fir];
        mtcOps[1] = ioMips->IoGPRs[ops.fir];
        ioMips->cop.mtc(static_cast<u8>(mtcOps[0]), mtcOps[1]);
    }

    void IopInterpreter::rfe(Operands ops) {
        ioMips->cop.status.kuc = ioMips->cop.status.kup;
        ioMips->cop.status.kup = ioMips->cop.status.kuo;

        ioMips->cop.status.iec = ioMips->cop.status.iep;
        ioMips->cop.status.iep = ioMips->cop.status.ieo;
    }

    void IopInterpreter::sltiu(Operands ops) {
        u32* gprSrc = &ioMips->IoGPRs[ops.thi];
        u32* gprDest = &ioMips->IoGPRs[ops.sec];
        u8 opp{static_cast<u8>(ops.operation.pa8[3] >> 2)};
        if (opp == Slti) {
            i32 imm{ops.operation.sins & 0xffff};
            *gprDest = *gprSrc < imm;
        } else if (opp == Sltiu) {
            u32 imm{ops.operation.inst & 0xffff};
            *gprDest = *gprSrc < imm;
        }
    }
    void IopInterpreter::orSMips(Operands ops) {
        ioMips->IoGPRs[ops.fir] = ioMips->IoGPRs[ops.thi] | ioMips->IoGPRs[ops.sec];
    }
    void IopInterpreter::xorSMips(Operands ops) {
        ioMips->IoGPRs[ops.fir] = ioMips->IoGPRs[ops.thi] ^ ioMips->IoGPRs[ops.sec];
    }

    void IopInterpreter::nor(Operands ops) {
        orSMips(ops);
        ioMips->IoGPRs[ops.fir] = ~ioMips->IoGPRs[ops.fir];
    }
    void IopInterpreter::ioSyscall(Operands ops) {
        ioMips->cop.cause.code = 0x8;
        raw_reference<vm::EmuVM> vm{redBox->openVm()};
        vm->dealWithSyscalls();
        redBox->leaveVm(vm);
    }

    u32 IopInterpreter::execCopRow(u32 opcode, std::array<u8, 3> opeRegs) {
        u16 cop{static_cast<u16>((opcode >> 21) & 0x1f)};
        cop |= static_cast<u16>((opcode >> 26) & 0x3) << 8;
        switch (cop) {
        case CopMfc: mfc(Operands(opcode, opeRegs)); break;
        case CopMtc: mtc(Operands(opcode, opeRegs)); break;
        case CopRfe: rfe(Operands(opcode, opeRegs)); break;
        }
        return opcode;
    }
    u32 IopInterpreter::execIo3S(u32 opcode, std::array<u8, 3> opeRegs) {
        switch (opcode & 0x3f) {
        case SpecialSyscall: ioSyscall(Operands(opcode, opeRegs)); break;
        case SpecialMfhi: mfhi(Operands(opcode, opeRegs)); break;
        case SpecialMthi: mthi(Operands(opcode, opeRegs)); break;
        case SpecialOr: orSMips(Operands(opcode, opeRegs)); break;
        case SpecialXor: xorSMips(Operands(opcode, opeRegs)); break;
        case SpecialNor: nor(Operands(opcode, opeRegs)); break;
        }
        return opcode;
    }
    u32 IopInterpreter::execIo3(u32 opcode, std::array<u8, 3> opeRegs) {
        switch (opcode >> 26) {
        case SpecialOp: return execIo3S(opcode, opeRegs);
        case Bne: bne(Operands(opcode, opeRegs)); break;
        case Blez: blez(Operands(opcode, opeRegs)); break;
        case 0x10 ... 0x13: return execCopRow(opcode, opeRegs);
        case Slti:
        case Sltiu:
            sltiu(Operands(opcode, opeRegs)); break;
        default:
            ;
        }
        return opcode;
    }
    void IopInterpreter::issueInterruptSignal() {
        if (ioMips->cop.status.iec && (ioMips->cop.status.imm & ioMips->cop.cause.intPending)) {
            ioMips->handleException(0);
        }
    }
    u32 IopInterpreter::executeCode() {
        u32 opcode{};
        std::array<u8, 3> opes;
        do {
            if (ioMips->irqSpawned) {
                if (ioMips->mathDelay)
                    ioMips->mathDelay--;
                issueInterruptSignal();
            }
            if (ioMips->cyclesToIo < 0)
                break;
            ioMips->cyclesToIo--;
            opcode = fetchPcInst();
            opes[0] = (opcode >> 11) & 0x1f;
            opes[1] = (opcode >> 16) & 0x1f;
            opes[2] = (opcode >> 21) & 0x1f;

            execIo3(opcode, opes);
            if (ioMips->onBranch) {
                if (!ioMips->branchDelay) {
                    ioMips->lastPc = ioMips->ioPc;
                    ioMips->ioPc = ioMips->waitPc;
                    ioMips->onBranch = false;
                } else {
                    ioMips->branchDelay--;
                }
            }
        } while (ioMips->cyclesToIo > 0);
        return opcode;
    }
    static std::array<u32, 3> pcPutC{0x00012c48, 0x0001420c, 0x0001430c};
    u32 IopInterpreter::fetchPcInst() {
        u32 inst{ioMips->fetchByPC()};
        std::array<u32, 2> hookPs{ioMips->IoGPRs[5], ioMips->IoGPRs[6]};
        fmt::memory_buffer iosBuffer{};
        mio::VirtualPointer start, end;

        // Hooking all parameters of the putc function
        if (ranges::any_of(pcPutC, [inst](auto address) { return address == inst; })) {
            start = ioMips->iopMem->solveGlobal(hookPs[0]);
            end = ioMips->iopMem->solveGlobal(hookPs[0] + hookPs[1]);
            iosBuffer.append(start.as<const char*>(), end.as<const char*>());
        }
        if (iosBuffer.size()) {
            userLog->info("(IOP): putc function call intercepted, parameters {::#x} and {}, text {}",
                hookPs[0], hookPs[1], iosBuffer.data());
        }
        return inst;
    }
}
