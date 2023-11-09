#include <eeiv/ee_engine.h>

namespace zenith::eeiv {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsign-conversion"
    void EEMipsCore::verifyAndBranch(bool cond, i32 jumpRel) {
        if (cond) {
            isABranch = cond;
            eePC = *eePC + jumpRel + 4;
            delaySlot = 1;
        }
    }
#pragma clang diagnostic pop
    void EEMipsCore::updateTlb() {
        tlbMap = cop0.mapVirtualTLB(eeTLB);
    }

    mio::TLBPageEntry* EEMipsCore::fetchTLBFromCop(u32* c0Regs) {
        u16 c0id{*reinterpret_cast<u16*>(c0Regs[0])};
        return &eeTLB->tlbInfo[c0id];
    }
}

