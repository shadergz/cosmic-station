#include <common/global.h>
#include <iop/iop_core.h>
#include <fuji/iop_interpreter.h>

namespace cosmic::iop {
    void IoMipsCore::intByINTC(bool isInt) {
        // Check or uncheck interrupt switch
        if (isInt)
            cop.cause.intPending |= 0x4;
        else
            cop.cause.intPending &= ~0x4;
    }

    IoMipsCore::IoMipsCore(std::shared_ptr<mio::GlobalMemory>& mem)
        : iopMem(mem) {
        interpreter = std::make_unique<fuji::IOPInterpreter>(*this);
        for (auto& ic : iCache) {
            ic.data = ic.tag = 0;
            ic.isValid = false;
        }
    }
    u8* IoMipsCore::iopPrivateAddrSolver(u32 address) {
        if (address >= 0x1f900000 && address < 0x1f900400) {
            // SPU2 registers
        } else if (address >= 0x1d000000 && address < 0x1f800000) {
            // SIF registers
        }
        return nullptr;
    }
    void IoMipsCore::takeBranchIf(bool take, i32 pcAddr) {
        if (!take && !onBranch)
            return;
        i64 calcPc{static_cast<i64>(ioPc) + 4 + pcAddr};
        waitPc = static_cast<u32>(calcPc);
        if (waitPc & 0x3) {
            throw AppFail("Next IOP PC {::x}: lowest 3 bits couldn't be set", waitPc);
        }
        onBranch = true;
        branchDelay = 1;
    }

    void IoMipsCore::resetIOP() {
        // The IOP processor initializes the PC at the same address as the EE
        ioPc = 0xbfc00000;
        lastPc = waitPc = 0;

        std::memset(IOGPRs.data(), 0, sizeof(IOGPRs));
        irqSpawned = cyclesToIO = 0;
        hi = lo = 0;
        cacheCtrl = 0;
        mathDelay = branchDelay = 0;
        onBranch = false;
        userLog->info("(IOP): Reset finished, IOP->PC: {}", ioPc);
    }
    void IoMipsCore::pulse(u32 cycles) {
        cyclesToIO += cycles;
        if (!irqSpawned && cyclesToIO) {
            interpreter->executeCode();
        } else if (cop.status.iec && (cop.status.imm & cop.cause.intPending)) {
            handleException(0);
        }
    }
    u32 IoMipsCore::fetchByPC() {
        lastPc = ioPc;
        if (ioPc >= 0xa0000000 || !(cacheCtrl & (1 << 11))) {
            // Reading directly from IO RAM incurs a penalty of 4 machine cycles
            cyclesToIO -= 4;
            mathDelay = std::max(mathDelay - 4, 0);
        }
        const u32 ioOpcode{iopRead<u32>(ioPc)};
        ioPc += 4;
        return ioOpcode;
    }
    static std::array<u32, 2> exceptionAddr{0x80000080, 0xbfc00180};
    const u8 busError{0x4};
    void IoMipsCore::handleException(u8 code) {
        cop.cause.code = code;
        if (onBranch)
            cop.ePC = ioPc - 4;
        else
            cop.ePC = ioPc;
        cop.cause.bd = onBranch;
        cop.status.ieo = cop.status.iep;
        cop.status.iep = cop.status.iec;
        cop.status.iec = false;
        if (code == busError) {
            // R2-R3 or v0-v1 -> Subroutine return values, may be changed by subroutines
            if (!(ioPc & 0x3 || IOGPRs[2] & 0x1 || IOGPRs[3] & 0x1))
                if (!(IOGPRs[28] & 0x1 || IOGPRs[29] & 0x1))
                    ;
        }
        // There are only two exception handler addresses,
        // and we can decide by looking the bootstrap status
        ioPc = exceptionAddr[cop.status.bev ? 1 : 0];
        onBranch = false;
    }
}
