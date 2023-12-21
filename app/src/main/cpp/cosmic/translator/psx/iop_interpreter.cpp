#include <range/v3/algorithm.hpp>
#include <console/backdoor.h>
#include <common/global.h>
#include <translator/psx/iop_interpreter.h>
#include <vm/emu_vm.h>

namespace cosmic::translator::psx {
    using namespace iop;
    void IopInterpreter::bne(Operands ops) {
        ioMips->takeBranchIf(ioMips->IoGPRs[ops.rs] != ioMips->IoGPRs[ops.rt],
            (ops.operation.sins & 0xffff) << 2);
    }
    void IopInterpreter::blez(Operands ops) {
        ioMips->takeBranchIf(ioMips->IoGPRs[ops.rs] <= 0, (ops.operation.sins & 0xffff) << 2);
    }
    void IopInterpreter::mfhi(Operands ops) {
        u32 target{ioMips->IoGPRs[ops.rd]};
        ioMips->IoGPRs[target] = ioMips->hi;
    }
    void IopInterpreter::mthi(Operands ops) {
        ioMips->hi = ioMips->IoGPRs[ioMips->IoGPRs[ops.rs]];
    }

    void IopInterpreter::mfc(Operands ops) {
        if (((ops.operation.pa8[3]) & 0x3) > 0)
            ;
        u32 fetched{ioMips->cop.mfc(ops.rd)};
        ioMips->IoGPRs[ops.rt] = fetched;
    }

    void IopInterpreter::mtc(Operands ops) {
        std::array<u32, 2> mtcOps;

        mtcOps[0] = ioMips->IoGPRs[ops.rd];
        mtcOps[1] = ioMips->IoGPRs[ops.rd];
        ioMips->cop.mtc(static_cast<u8>(mtcOps[0]), mtcOps[1]);
    }

    void IopInterpreter::rfe(Operands ops) {
        ioMips->cop.status.kuc = ioMips->cop.status.kup;
        ioMips->cop.status.kup = ioMips->cop.status.kuo;

        ioMips->cop.status.iec = ioMips->cop.status.iep;
        ioMips->cop.status.iep = ioMips->cop.status.ieo;
    }

    void IopInterpreter::sltiu(Operands ops) {
        u32* gprs[0x2];

        gprs[0] = &ioMips->IoGPRs[ops.rs];
        gprs[1] = &ioMips->IoGPRs[ops.rt];
        u8 opp{static_cast<u8>(ops.operation.pa8[3] >> 2)};
        if (opp == Slti) {
            i32 imm{ops.operation.sins & 0xffff};
            *gprs[1] = *gprs[0] < imm;
        } else if (opp == Sltiu) {
            u32 smm{ops.operation.inst & 0xffff};
            *gprs[1] = *gprs[0] < smm;
        }
    }
    void IopInterpreter::orSMips(Operands ops) {
        ioMips->IoGPRs[ops.rd] = ioMips->IoGPRs[ops.rs] | ioMips->IoGPRs[ops.rt];
    }
    void IopInterpreter::xorSMips(Operands ops) {
        ioMips->IoGPRs[ops.rd] = ioMips->IoGPRs[ops.rs] ^ ioMips->IoGPRs[ops.rt];
    }

    void IopInterpreter::nor(Operands ops) {
        orSMips(ops);
        ioMips->IoGPRs[ops.rd] = ~ioMips->IoGPRs[ops.rd];
    }
    void IopInterpreter::ioSyscall(Operands ops) {
        ioMips->cop.cause.code = 0x8;
        raw_reference<vm::EmuVM> vm{redBox->openVm()};
        vm->dealWithSyscalls();
        redBox->leaveVm(vm);
    }

    u32 IopInterpreter::execCopRow(u32 opcode, std::array<u8, 3> opeRegs) {
        u16 cop{static_cast<u16>((opcode >> 21) & 0x1f)};
        cop |= static_cast<u8>((opcode >> 26) & 0x3) << 8;
        switch (cop) {
        case CopMfc: mfc(Operands(opcode, opeRegs)); break;
        case CopMtc: mtc(Operands(opcode, opeRegs)); break;
        case CopRfe: rfe(Operands(opcode, opeRegs)); break;
        }
        return opcode;
    }
    u32 IopInterpreter::execIo3s(u32 opcode, std::array<u8, 3> opeRegs) {
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
        case SpecialOp: return execIo3s(opcode, opeRegs);
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
    u32 IopInterpreter::fetchPcInst() {
        u32 instr[1];
        u32 ipc{ioMips->ioPc};

        if (fastPc.isFastMemoryEnb && ioMips->isPcUncached(ipc)) {
            u32 pc{ioMips->translateAddr(ipc)};
            if (!fastPc.checkPc(pc)) {
                if (ioMips->isRoRegion(pc)) {
                    auto virtPc{ioMips->pipeRead<u8*>(pc)};
                    fastPc.pushVpc(pc, virtPc);
                }
            }
            auto [val, isValid]{fastPc.fastFetch(pc)};
            if (isValid) {
                instr[0] = val;
                ioMips->incPc();
            } else {
                instr[0] = ioMips->fetchByPc();
            }
        } else {
            instr[0] = ioMips->fetchByPc();
        }
        if ((ipc - 0xa0000000) >= 0x1fc00000)
            ioFuncHook(ipc - (0xa0000000 + 0x1fc00000));
        return instr[0];
    }

    const std::array<u32, 3> pcPutC{0x00012c48, 0x0001420c, 0x0001430c};
    void IopInterpreter::ioFuncHook(u32 pc) {
        std::array<u32, 2> hookPs{ioMips->IoGPRs[5],
            ioMips->IoGPRs[6]};
        fmt::memory_buffer iosBuffer{};
        mio::VirtualPointer
            start{},
            end{};

        bool isPutc{ranges::any_of(pcPutC, [pc](auto address) { return address == pc; })};
        if (isPutc) {
            // Hooking all parameters of the putc function
            start = ioMips->iopMem->solveGlobal(hookPs[0]);
            end = ioMips->iopMem->solveGlobal(hookPs[0] + hookPs[1]);
        }
        if (start && end) {
            iosBuffer.append(start.as<const char *>(), end.as<const char *>());
            userLog->info("(IOP): putc function call intercepted, parameters {:x}, text {}",
                fmt::join(hookPs, ", "), fmt::to_string(iosBuffer));
        }
    }
}
