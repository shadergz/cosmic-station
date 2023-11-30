#include <arm_neon.h>
#include <eeiv/copctrl/cop0.h>

namespace cosmic::eeiv::copctrl {
    CoProcessor0::CoProcessor0(std::shared_ptr<mio::DMAController>& ctrl)
        : dmac(ctrl) {
        iCacheLines = new CopCacheLine[countOfCacheLines];
        // Invalidating all cache lines
        for (u8 line{}; line < countOfCacheLines; line++) {
            iCacheLines[line].tags[0] = invCacheBit;
            iCacheLines[line].tags[1] = invCacheBit;

            iCacheLines[line].lrf[0] = false;
            iCacheLines[line].lrf[1] = false;
        }
    }
    bool CoProcessor0::isIntEnabled() {
        return !status.exception && !status.error;
    }
    void CoProcessor0::incPerfByEvent(u32 mask, u32 cycles, u8 perfEv) {
        bool canCount{false};

        enum PerfMetrics {
            ProcessorCycle = 1,
            SingleDoubleIssue,
            BranchIssueOrMispredicted,
            InstructionFinished = 0xc,
            NonDelaySlotInstruction,
            Cop2OrCop1Instruction,
            LoadStoreInstruction,
        };
        if (perfCounter & mask) {
            u32 metric{static_cast<u32>(perfCounter >> perfEv ? 15 : 5) & 0x1f};
            switch (metric) {
            case ProcessorCycle ... BranchIssueOrMispredicted:
            case InstructionFinished ... LoadStoreInstruction:
                canCount = true;
            }
        }
        if (canCount) {
            if (!perfEv)
                perf0 += cycles;
            else if (perfEv == 1)
                perf1 += cycles;
        }
    }

    void CoProcessor0::rectifyTimer(u32 pulseCycles) {
        if (GPRs[9] + pulseCycles >= GPRs[11] && GPRs[9] < GPRs[11])
            cause.timerIP = true;
        count += pulseCycles;

        if (perfCounter & static_cast<u32>(1 << 31)) {
            u32 pcrMask{static_cast<u32>((1 << (status.mode + 2)) | (status.exception << 1))};

            incPerfByEvent(pcrMask, pulseCycles, 0);
            incPerfByEvent(pcrMask << 0xa, pulseCycles, 1);
        }
    }
    CoProcessor0::~CoProcessor0() {
        delete[] iCacheLines;
    }

    u8** CoProcessor0::mapVirtualTLB(std::shared_ptr<mio::TlbCache>& virtTable) {
        if (status.exception || status.error || status.mode == KSU::kernel)
            return virtTable->kernelVTLB;

        switch (status.mode) {
        case KSU::supervisor:
            return virtTable->supervisorVTLB;
        case KSU::user:
            return virtTable->userVTLB;
        default:
            return virtTable->kernelVTLB;
        }
    }
    // https://rust-console.github.io/ps2-bios-book
    // https://psi-rockin.github.io/ps2tek/#biosbootprocess
    void CoProcessor0::resetCoP() {
        status.bev = true;
        status.usable = 0x7;
        perf0 = perf1 = 0;

        for (u8 regs{}; regs != cop0RegsCount; regs += 8) {
            u256 zero{};
            vst1_u64_x4(bit_cast<u64*>(GPRs.data() + regs), zero);
        }
        // Co-processor revision ID
        pRid = 0x2e59;
    }
}