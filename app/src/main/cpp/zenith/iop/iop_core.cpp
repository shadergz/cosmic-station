#include <common/global.h>
#include <iop/iop_core.h>
#include <fuji/iop_interpreter.h>

namespace zenith::iop {
    void IOMipsCore::intByINTC(bool isInt) {
        if (isInt)
            cop.cause.intPending |= 0x4;
        else
            cop.cause.intPending &= ~0x4;
    }

    IOMipsCore::IOMipsCore(std::shared_ptr<mio::GlobalMemory>& mem)
        : iopMem(mem) {
        interpreter = std::make_unique<fuji::IOPInterpreter>(*this);
        for (auto& ic : iCache) {
            ic.data = ic.tag = 0;
            ic.isValid = false;
        }
    }
    u8* IOMipsCore::iopPrivateAddrSolver(u32 address) {
        if (address >= 0x1f900000 && address < 0x1f900400) {
            // SPU2 registers
        } else if (address >= 0x1d000000 && address < 0x1f800000) {
            // SIF registers
        }
        return nullptr;
    }
    void IOMipsCore::resetIOP() {
        // The IOP processor initializes the PC at the same address as the EE
        ioPc = 0xbfc00000;
        std::memset(IOGPRs.data(), 0, sizeof(IOGPRs));
        irqSpawned = cyclesToIO = 0;
        hi = lo = 0;

        userLog->info("(IOP): Reset finished, IOP->PC: {}", ioPc);
    }
    void IOMipsCore::pulse(u32 cycles) {
        cyclesToIO += cycles;
        if (!irqSpawned && cyclesToIO) {
            interpreter->executeCode();
        }
    }
    u32 IOMipsCore::fetchByPC() {
        u32 ioOpcode{iopRead<u32>(ioPc)};
        lastPC = ioPc;
        ioPc += 4;
        return ioOpcode;
    }
}
