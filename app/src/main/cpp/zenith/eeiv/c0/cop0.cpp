#include <arm_neon.h>
#include <eeiv/c0/cop0.h>

namespace zenith::eeiv {
    CoProcessor0::CoProcessor0() {
        // Signals to the BIOS that the EE is in its boot process, so it will start our registers
        // Co-processor revision ID
        pRid = 0x59;
        eeNearCache = new EECacheLine[countOfCacheLines];

        // Invalidating all cache lines
        for (u8 line{}; line < countOfCacheLines; line++) {
            eeNearCache[line].tags[0] = invCacheBit;
            eeNearCache[line].tags[1] = invCacheBit;

            eeNearCache[line].lrf[0] = false;
            eeNearCache[line].lrf[1] = false;
        }
    }
    CoProcessor0::~CoProcessor0() {
        delete[] eeNearCache;
    }

    u8** CoProcessor0::mapVirtualTLB(std::shared_ptr<TLBCache>& virtTable) {
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

    void CoProcessor0::resetCoP() {
        status.copUsable = 0x7;

        for (u8 regs{}; regs != cop0RegsCount; regs += 8) {
            uint32x2x4_t zero{};
            vst1_u32_x4(copGPRs + regs, zero);
        }
    }
}