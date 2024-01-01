#include <arm_neon.h>
#include <engine/copctrl/cop0.h>

namespace cosmic::engine::copctrl {
    CtrlCop::CtrlCop(std::shared_ptr<mio::DmaController>& ctrl) :
        dmac(ctrl) {
        // Invalidating all cache lines
        std::memset(dataCache.data(), 0, sizeof(dataCache));
        std::memset(inCache.data(), 0, sizeof(inCache));

        for (u8 cl{}; cl < countOfCacheLines; cl++) {
            inCache[cl].tags.fill(~validBit);
            if (cl < 64)
                dataCache[cl].tags.fill(~validBit);
        }
        virtCache = std::make_shared<mio::TlbCache>(dmac->mapped);
    }
    bool CtrlCop::isIntEnabled() {
        return !status.exception && !status.error;
    }
    void CtrlCop::incPerfByEvent(u32 mask, u32 cycles, u8 perfEv) {
        bool canCount{false};

        enum PerfMetrics {
            ProcessorCycle = 0x1,
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

    void CtrlCop::rectifyTimer(u32 pulseCycles) {
        if (GPRs[9] + pulseCycles >= GPRs[11] && GPRs[9] < GPRs[11])
            cause.timerIp = true;
        count += pulseCycles;

        if (perfCounter & static_cast<u32>(1 << 31)) {
            u32 pcrMask{static_cast<u32>((1 << (status.mode + 2)) | (status.exception << 1))};

            incPerfByEvent(pcrMask, pulseCycles, 0);
            incPerfByEvent(pcrMask << 0xa, pulseCycles, 1);
        }
    }
    CtrlCop::~CtrlCop() {
    }

    void CtrlCop::redoTlbMapping() {
        if (status.exception || status.error || status.mode == Ksu::Kernel)
            virtMap = virtCache->kernelVtlb;

        switch (status.mode) {
        case Ksu::Supervisor:
            virtMap = virtCache->supervisorVtlb;
        case Ksu::User:
            virtMap = virtCache->userVtlb;
        default:
            virtMap = virtCache->kernelVtlb;
        }
    }
    // https://rust-console.github.io/ps2-bios-book
    // https://psi-rockin.github.io/ps2tek/#biosbootprocess
    void CtrlCop::resetCoP() {
        status.bev = true;
        status.usable = 0x7;
        perf0 = perf1 = 0;

        for (u8 regs{}; regs != cop0RegsCount; regs += 8) {
            u256 zero{};
            vst1_u64_x4(BitCast<u64*>(GPRs.data() + regs), zero);
        }
        // Co-processor revision ID
        pRid = 0x2e59;
    }
}