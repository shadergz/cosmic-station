#include <range/v3/algorithm.hpp>
#include <console/backdoor.h>
#include <common/global.h>
#include <creeper/psx/iop_interpreter.h>
#include <vm/emu_vm.h>

namespace cosmic::creeper::psx {
    using namespace iop;
    void IopInterpreter::bne(Operands ops) {
        ioMips->takeBranchIf(ioMips->ioGPRs[ops.rs] != ioMips->ioGPRs[ops.rt],
            (ops.sins & 0xffff) << 2);
    }
    void IopInterpreter::blez(Operands ops) {
        ioMips->takeBranchIf(ioMips->ioGPRs[ops.rs] <= 0, (ops.sins & 0xffff) << 2);
    }
    void IopInterpreter::mfhi(Operands ops) {
        u32 target{ioMips->ioGPRs[ops.rd]};
        ioMips->ioGPRs[target] = ioMips->hi;
    }
    void IopInterpreter::mthi(Operands ops) {
        ioMips->hi = ioMips->ioGPRs[ioMips->ioGPRs[ops.rs]];
    }
    void IopInterpreter::jr(Operands ops) {
        // A instruction that is in the branch delay slot is executed before the PC is altered
        // (at the end of the jr instruction)
        ioMips->jumpTo(ops.rs);
    }
    void IopInterpreter::beq(Operands ops) {
        // Sometimes, if called with ops.rs and ops.rt equal, it is just an unconditional jump
        // Just like this: b 0x30 == beq $zero, $zero, 0x30
        bool take{ioMips->ioGPRs[ops.rs] == ioMips->ioGPRs[ops.rt]};
        i32 jumpOffset{(ops.sins & 0xffff) << 2};
        ioMips->branchIf(take, jumpOffset);
    }

    void IopInterpreter::mfc(Operands ops) {
        if (((ops.pa8[3]) & 0x3) > 0)
            ;
        u32 fetched{ioMips->cop.mfc(ops.rd)};
        ioMips->ioGPRs[ops.rt] = fetched;
    }

    void IopInterpreter::mtc(Operands ops) {
        std::array<u32, 2> mtcOps;

        mtcOps[0] = ioMips->ioGPRs[ops.rd];
        mtcOps[1] = ioMips->ioGPRs[ops.rd];
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

        gprs[0] = &ioMips->ioGPRs[ops.rs];
        gprs[1] = &ioMips->ioGPRs[ops.rt];
        u8 opp{static_cast<u8>(ops.pa8[3] >> 2)};

        i32 imm{ops.sins & 0xffff};
        u32 smm{ops.inst & 0xffff};
        if (opp == Slti)
            *gprs[1] = *gprs[0] < imm;
        else if (opp == Sltiu)
            *gprs[1] = *gprs[0] < smm;
    }
    void IopInterpreter::ioSyscall(Operands ops) {
        ioMips->cop.cause.code = 0x8;
        vm->dealWithSyscalls();
    }

    u32 IopInterpreter::execCopRow(u32 opcode, std::array<u8, 3> opeRegs) {
        u16 cop{static_cast<u16>((opcode >> 21) & 0x1f)};
        cop |= static_cast<u8>((opcode >> 26) & 0x3) << 8;
        auto copArgs{Operands(opcode, opeRegs)};
        switch (cop) {
        case CopMfc: mfc(copArgs); break;
        case CopMtc: mtc(copArgs); break;
        case CopRfe: rfe(copArgs); break;
        }
        return opcode;
    }
    u32 IopInterpreter::execIo3s(u32 opcode, std::array<u8, 3> opeRegs) {
        auto instArgs{Operands(opcode, opeRegs)};

        switch (opcode & 0x3f) {
        case SpecialJr: jr(instArgs); break;
        case SpecialSyscall: ioSyscall(instArgs); break;
        case SpecialMfhi: mfhi(instArgs); break;
        case SpecialMthi: mthi(instArgs); break;
        case SpecialOr: orMips(instArgs); break;
        case SpecialXor: xorMips(instArgs); break;
        case SpecialNor: nor(instArgs); break;
        }
        return opcode;
    }
    u32 IopInterpreter::execIo3(u32 opcode, std::array<u8, 3> opeRegs) {
        auto ioArgs{Operands(opcode, opeRegs)};
        switch (opcode >> 26) {
        case SpecialOp: return execIo3s(opcode, opeRegs);
        case Beq: beq(ioArgs); break;
        case Bne: bne(ioArgs); break;
        case Blez: blez(ioArgs); break;
        case Addi: addi(ioArgs); break;
        case Addiu: addiu(ioArgs); break;
        case Slti:
        case Sltiu: sltiu(ioArgs); break;
        case Andi: andi(ioArgs); break;
        case Ori: ori(ioArgs); break;
        case Lui: lui(ioArgs); break;
        case 0x10 ... 0x13: return execCopRow(opcode, opeRegs);
        case Lw: lw(ioArgs); break;
        case Sw: sw(ioArgs); break;
        default:
            ;
        }
        return opcode;
    }
    void IopInterpreter::issueInterruptSignal() {
        if (ioMips->cop.status.iec && (
                ioMips->cop.status.imm &
                ioMips->cop.cause.intPending)) {
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
            // [First bytes of the IOP initialization subroutine]
            // 0x3c1abfc0 = lui $k0, 0xbfc0
            // 0x375A2000 = ori $k0, $k0, 0x2000 # 0xbfc20000 ? Bootstrap address
            // = jr $k0
            // 0x00000000 = nop

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
                    auto virtPc{ioMips->iopMem->solveGlobal(pc, mio::IopDev).as<u8*>()};
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
        std::array<u32, 2> hookPs{ioMips->ioGPRs[5],
            ioMips->ioGPRs[6]};
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
    IopInterpreter::IopInterpreter(
            raw_reference<iop::IoMipsCore> core) :
        IopExecVe(core) {
        raw_reference<vm::EmuVm> vmInter{redBox->openVm()};

        vm = vmInter;
        redBox->leaveVm(vm);
    }
}
