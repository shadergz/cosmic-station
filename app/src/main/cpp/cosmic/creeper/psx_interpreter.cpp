#include <range/v3/algorithm.hpp>
#include <console/backdoor.h>
#include <common/global.h>
#include <creeper/psx_interpreter.h>
#include <vm/emu_vm.h>

namespace cosmic::creeper {
    using namespace iop;
    void IopInterpreter::bne(Operands ops) {
        cpu->takeBranchIf(cpu->ioGPRs[ops.rs] != cpu->ioGPRs[ops.rt],
            (ops.sins & 0xffff) << 2);
    }
    void IopInterpreter::blez(Operands ops) {
        cpu->takeBranchIf(cpu->ioGPRs[ops.rs] <= 0, (ops.sins & 0xffff) << 2);
    }
    void IopInterpreter::mfhi(Operands ops) {
        u32 target{cpu->ioGPRs[ops.rd]};
        cpu->ioGPRs[target] = cpu->hi;
    }
    void IopInterpreter::mthi(Operands ops) {
        cpu->hi = cpu->ioGPRs[cpu->ioGPRs[ops.rs]];
    }
    void IopInterpreter::jr(Operands ops) {
        // A instruction that is in the branch delay slot is executed before the PC is altered
        // (at the end of the jr instruction)
        cpu->jumpTo(ops.rs);
    }
    void IopInterpreter::beq(Operands ops) {
        // Sometimes, if called with ops.rs and ops.rt equal, it is just an unconditional jump
        // Just like this: b 0x30 == beq $zero, $zero, 0x30
        auto take{cpu->ioGPRs[ops.rs] == cpu->ioGPRs[ops.rt]};
        i32 jumpOffset{(ops.sins & 0xffff) << 2};
        cpu->branchIf(take, jumpOffset);
    }

    void IopInterpreter::mfc(Operands ops) {
        if (((ops.pa8[3]) & 0x3) > 0) {
        }
        u32 fetched{cpu->cop.mfc(ops.rd)};
        cpu->ioGPRs[ops.rt] = fetched;
    }

    void IopInterpreter::mtc(Operands ops) {
        std::array<u32, 2> mtcOps;

        mtcOps[0] = cpu->ioGPRs[ops.rd];
        mtcOps[1] = cpu->ioGPRs[ops.rd];
        cpu->cop.mtc(static_cast<u8>(mtcOps[0]), mtcOps[1]);
    }

    void IopInterpreter::rfe(Operands ops) {
        cpu->cop.status.kuc = cpu->cop.status.kup;
        cpu->cop.status.kup = cpu->cop.status.kuo;

        cpu->cop.status.iec = cpu->cop.status.iep;
        cpu->cop.status.iep = cpu->cop.status.ieo;
    }

    void IopInterpreter::sltiu(Operands ops) {
        std::array<u32*, 2> gprs;

        gprs[0] = &cpu->ioGPRs[ops.rs];
        gprs[1] = &cpu->ioGPRs[ops.rt];
        u8 opp{static_cast<u8>(ops.pa8[3] >> 2)};

        auto imm{ops.sins & 0xffff};
        auto smm{ops.inst & 0xffff};
        if (opp == Slti)
            *gprs[1] = *gprs[0] < imm;
        else if (opp == Sltiu)
            *gprs[1] = *gprs[0] < smm;
    }
    void IopInterpreter::ioSyscall(Operands ops) {
        cpu->cop.cause.code = 0x8;
        vm->dealWithSyscalls();
    }

    u32 IopInterpreter::execCop(u32 opcode, std::array<u8, 3> opeRegs) {
        auto cop{static_cast<u16>((opcode >> 21) & 0x1f)};
        cop |= static_cast<u8>((opcode >> 26) & 0x3) << 8;
        auto copArgs{Operands(opcode, opeRegs)};
        switch (cop) {
        case CopMfc: mfc(copArgs); break;
        case CopMtc: mtc(copArgs); break;
        case CopRfe: rfe(copArgs); break;
        }
        return opcode;
    }
    u32 IopInterpreter::execSpecial(u32 opcode, std::array<u8, 3> opeRegs) {
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
    u32 IopInterpreter::execPsx(u32 opcode, std::array<u8, 3> opeRegs) {
        auto ioArgs{Operands(opcode, opeRegs)};
        switch (opcode >> 26) {
        case SpecialOp:
            return execSpecial(opcode, opeRegs);
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
        case 0x10 ... 0x13:
            return execCop(opcode, opeRegs);
        case Lw: lw(ioArgs); break;
        case Sw: sw(ioArgs); break;
        default: {}
        }
        return opcode;
    }
    void IopInterpreter::issueInterruptSignal() {
        if (cpu->cop.status.iec && (
                cpu->cop.status.imm &
                cpu->cop.cause.intPending)) {
            cpu->handleException(0);
        }
    }
    u32 IopInterpreter::executeCode() {
        u32 opcode{};
        std::array<u8, 3> opes;
        do {
            if (cpu->irqSpawned) {
                if (cpu->mathDelay)
                    cpu->mathDelay--;
                issueInterruptSignal();
            }
            if (cpu->cyclesToIo < 0)
                break;
            cpu->cyclesToIo--;
            // FIXME: The IOP increments the PC in its fetch function ????
            opcode = fetchPcInst();
            opes[0] = (opcode >> 11) & 0x1f;
            opes[1] = (opcode >> 16) & 0x1f;
            opes[2] = (opcode >> 21) & 0x1f;
            // [First bytes of the IOP initialization subroutine]
            // 0x3c1abfc0 = lui $k0, 0xbfc0
            // 0x375A2000 = ori $k0, $k0, 0x2000 # 0xbfc20000 ? Bootstrap address
            // = jr $k0
            // 0x00000000 = nop

            execPsx(opcode, opes);
            if (cpu->onBranch) {
                if (!cpu->branchDelay) {
                    cpu->lastPc = cpu->ioPc;
                    cpu->ioPc = cpu->waitPc;
                    cpu->onBranch = false;
                } else {
                    cpu->branchDelay--;
                }
            }
        } while (cpu->cyclesToIo > 0);
        return opcode;
    }
    u32 IopInterpreter::fetchPcInst() {
        u32 instr[1];
        const u32 ipc{cpu->ioPc};
        // Operations using FastPC do not use the CPU cache

        if (fastPc.isFastMemoryEnb && cpu->isPcUncached(ipc)) {
            const u32 pc{cpu->translateAddr(ipc)};
            auto [val, isValid]{fastPc.fastFetch(pc)};
            [[unlikely]] if (!isValid) {
                if (cpu->isRoRegion(pc)) {
                    auto virtPc{cpu->iopMem->solveGlobal(pc, mio::IopDev).as<u8*>()};
                    fastPc.pushVpc(pc, virtPc);

                    auto [opcode, _]{fastPc.fastFetch(pc)};
                    instr[0] = opcode;
                } else {
                    instr[0] = cpu->fetchByPc();
                }
            } else {
                instr[0] = val;
                cpu->incPc();
            }
        } else {
            instr[0] = cpu->fetchByPc();
        }
        if ((ipc - 0xa0000000) >= 0x1fc00000)
            ioFuncHook(ipc - (0xa0000000 + 0x1fc00000));
        return instr[0];
    }

    const std::array<u32, 3> pcPutC{0x00012c48, 0x0001420c, 0x0001430c};
    void IopInterpreter::ioFuncHook(u32 pc) {
        std::array<u32, 2> hookPs{
            cpu->ioGPRs[5],
            cpu->ioGPRs[6]};
        fmt::memory_buffer iosBuffer{};
        mio::VirtualPointer
            start{},
            end{};

        bool isPutc{ranges::any_of(pcPutC, [pc](auto address) { return address == pc; })};
        if (isPutc) {
            // Hooking all parameters of the putc function
            start = cpu->iopMem->solveGlobal(hookPs[0]);
            end = cpu->iopMem->solveGlobal(hookPs[0] + hookPs[1]);
        }
        if (start && end) {
            iosBuffer.append(start.as<const char *>(), end.as<const char *>());
            user->info("(IOP): putc function call intercepted, parameters {:#x}, text {}",
                fmt::join(hookPs, ", "), fmt::to_string(iosBuffer));
        }
    }
    IopInterpreter::IopInterpreter(
            Wrapper<iop::IoMipsCore> core) :
        IopExecVe(core) {
        Wrapper<vm::EmuVm> vmInter{outside->openVm()};

        vm = vmInter;
        outside->leaveVm(vm);
    }
}
