#include <arm_neon.h>
#include <eeiv/cop0.h>

namespace zenith::eeiv {
    CoProcessor0::CoProcessor0() {
        // Signals to the BIOS that the EE is in its resetBIOS process, so it will start our registers
        // Co-processor revision ID
        pRid = 0x59;
    }

    u8** CoProcessor0::mapVirtualTLB(const std::shared_ptr<TLBCache>& tlb) {
        if (status.exception || status.error || status.mode == KSU::kernel)
            return tlb->kernelVTLB;

        switch (status.mode) {
        case KSU::supervisor:
            return tlb->supervisorVTLB;
        case KSU::user:
            return tlb->userVTLB;
        default:
            return tlb->kernelVTLB;
        }
    }

    void CoProcessor0::resetCoP() {
        status.copUsable = 0x7;

        for (u8 regs{}; regs != cop0RegsCount; regs += 8) {
            uint32x2x4_t zero{};
            vst1_u32_x4(copGPRs + regs, zero);
        }
    }
}