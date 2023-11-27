#include <eeiv/ee_engine.h>

namespace cosmic::eeiv {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsign-conversion"
    void EEMipsCore::branchByCondition(bool cond, i32 jumpRel) {
        if (cond) {
            isABranch = cond;
            eePC = *eePC + jumpRel + 4;
            delaySlot = 1;
        }
    }
    void EEMipsCore::branchOnLikely(bool cond, i32 jumpRel) {
        if (cond)
            branchByCondition(true, jumpRel);
        else
            chPC(*eePC + 4);
    }
#pragma clang diagnostic pop
    void EEMipsCore::updateTlb() {
        tlbMap = ctrl0.mapVirtualTLB(eeTLB);
    }
    void EEMipsCore::setTLBByIndex() {
        auto selectedLB{std::ref(eeTLB->entries[ctrl0.tlbIndex])};

        eeTLB->unmapTLB(selectedLB);
        ctrl0.setTLB(selectedLB);
        eeTLB->mapTLB(selectedLB);
    }

    mio::TLBPageEntry* EEMipsCore::fetchTLBFromCop(u32* c0Regs) {
        u16 c0id{*reinterpret_cast<u16*>(c0Regs[0])};
        return &eeTLB->entries[c0id];
    }
    void EEMipsCore::handleException(u8 el, u32 exceptVec, u8 code) {
        ctrl0.cause.exCode = code & 0xd;
        const u8 savePcId{static_cast<u8>(el == 1 ? 14 : 30)};

        if (isABranch) {
            ctrl0.mtc0(savePcId, *eePC - 4);
        } else {
            ctrl0.mtc0(savePcId, *eePC);
        }
        if (savePcId == 14) {
            ctrl0.cause.bd = isABranch;
            ctrl0.status.exception = true;
        } else {
            ctrl0.cause.bd2 = isABranch;
            ctrl0.status.error = true;
        }

        if (ctrl0.status.bev || ctrl0.status.dev) {
            exceptVec |= 0xbfc00;
            exceptVec += 200;
        }

        isABranch = false;
        chPC(exceptVec);
    }
}

