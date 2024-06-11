#include <range/v3/algorithm.hpp>
#include <common/global.h>
#include <iop/iop_core.h>
#include <creeper/psx_interpreter.h>
#include <pshook/hk_psx.h>

namespace cosmic::iop {
    void IoMipsCore::intByIntC(bool isInt) {
        // Check or uncheck interrupt switch
        if (isInt)
            cop.cause.intPending |= 0x4;
        else
            cop.cause.intPending &= ~0x4;
    }

    IoMipsCore::IoMipsCore(std::shared_ptr<mio::MemoryPipe>& pipe) :
        iopMem(pipe) {
            interpreter = std::make_unique<creeper::IopInterpreter>(*this);
        for (auto& cache : instCache) {
            cache.data = {};
            cache.tag = {};
            // cache.isValid = {};
        }
    }
    u32 IoMipsCore::iopPrivateAddrSolver(u32 address) {
        if (address >= 0x1f900000 && address < 0x1f900400) {
            // SPU2 registers
        } else if (address >= 0x1d000000 && address < 0x1f800000) {
            // SIF registers
        }
        return address;
    }
    void IoMipsCore::takeBranchIf(bool take, i32 pcAddr) {
        if (!take && !onBranch)
            return;
        i64 calcPc{static_cast<i64>(ioPc) + pcAddr};
        waitPc = static_cast<u32>(calcPc);
        if (waitPc & 0x3) {
            throw AppErr("Next IOP PC {:#x}: lowest 3 bits couldn't be set", waitPc);
        }
        onBranch = true;
        branchDelay = 1;
    }

    void IoMipsCore::resetIop() {
        // The IOP processor initializes the PC at the same address as the EE
        cop.resetIOCop();

        ioPc = 0xbfc00000;
        lastPc = waitPc = 0;
        cacheHit = cacheMiss = {};

        ranges::fill(ioGPRs, 0u);
        irqSpawned = cyclesToIo = {};
        hi = lo = 0;
        cacheCtrl = 0;
        mathDelay = branchDelay = 0;
        onBranch = false;
        ranges::for_each(instCache, [](auto& validLine) {
            validLine.isValid = {};
        });

        user->info("(IOP): Reset finished, IOP->PC: {}", ioPc);
    }
    void IoMipsCore::pulse(u32 cycles) {
        cyclesToIo += cycles;
        if (!irqSpawned && cyclesToIo) {
            interpreter->executeCode();
        } else if (cop.status.iec &&
            (cop.status.imm & cop.cause.intPending)) {
            handleException(0);
        }
    }
    u32 IoMipsCore::fetchByPc() {
        if (isPcUncached(ioPc)) {
            // Reading directly from IO RAM incurs a penalty of 4 machine cycles
            cyclesToIo -= 4;
            mathDelay = std::max(mathDelay - 4, 0);
            return iopRead<u32>(incPc());
        }
        const u32 tag{(ioPc & 0xffff'f000) >> 0xc};
        const u32 index{(ioPc & 0xffc) >> 2};
        if (index > instCache.size()) {
        }
        auto& ioc{instCache.at(index)};
        if (ioc.tag == tag && ioc.isValid) {
            cacheHit++;
            incPc();
            return ioc.data;
        }
        cacheMiss++;

        const u32 ioOpcode{iopRead<u32>(incPc())};
        ioc.data = ioOpcode;
        ioc.isValid = true;
        return ioOpcode;
    }
    u32 IoMipsCore::incPc() {
        lastPc = ioPc;
        ioPc += 4;
        return lastPc;
    }
    static std::array<u32, 2> exceptionAddr{0x8000'0080, 0xbfc0'0180};
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
            if (!(ioPc & 0x3 || ioGPRs[2] & 0x1 || ioGPRs[3] & 0x1))
                if (!(ioGPRs[28] & 0x1 || ioGPRs[29] & 0x1)) {
                }
        }
        // There are only two exception handler addresses,
        // and we can decide by looking the bootstrap status
        ioPc = exceptionAddr[cop.status.bev ? 1 : 0];
        onBranch = false;
        branchDelay = 0;

        u32 callPc{ioPc & 0x1fffff};
        if (callPc)
            pshook::EstablishHook::hookIoPsx(callPc, *this);
    }
    u32 IoMipsCore::translateAddr(u32 address) {
        // KSeg0
        if (address >= 0x8000'0000 && address < 0xa0000000)
            address -= 0x80000000;
        // KSeg1
        else if (address >= 0xa0000000 && address < 0xc0000000)
            address -= 0xa0000000;
        // KUSeg, KSeg2
        return address;
    }
    bool IoMipsCore::isPcUncached(u32 pc) const {
        return pc >= 0xa0000000 || !(cacheCtrl & (1 << 11));
    }
    bool IoMipsCore::isRoRegion(u32 address) {
        return address >= 0x1fc00000 && address < 0x20000000;
    }
    void IoMipsCore::jumpTo(u8 effectiveGpr) {
        if (onBranch)
            return;
        waitPc = ioGPRs[effectiveGpr];
        onBranch = true;
        branchDelay = 1;
    }
    void IoMipsCore::branchIf(bool cond, i32 offset) {
        if (!cond)
            return;
        if (offset < 0)
            waitPc = ioPc - static_cast<u32>(std::abs(offset));
        else
            waitPc = ioPc + static_cast<u32>(offset);
        onBranch = true;
        branchDelay = 1;
    }
}
