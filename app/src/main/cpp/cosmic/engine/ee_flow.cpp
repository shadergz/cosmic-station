#include <engine/ee_core.h>

namespace cosmic::engine {
    void EeMipsCore::branchByCondition(bool cond, i32 jumpRel) {
        if (!cond)
            return;
        isABranch = cond;
        i64 pc{static_cast<i64>(*eePc) + jumpRel + 4};
        eePc = static_cast<u32>(pc);
        delaySlot = 1;
    }
    void EeMipsCore::branchOnLikely(bool cond, i32 jumpRel) {
        if (cond)
            branchByCondition(true, jumpRel);
        else
            chPC(*eePc + 4);
    }
    void EeMipsCore::updateTlb() {
        tlbMap = ctrl0.mapVirtualTlb(eeTLB);
    }
    void EeMipsCore::setTlbByIndex() {
        auto selectedLb{std::ref(eeTLB->entries[ctrl0.tlbIndex])};

        eeTLB->unmapTlb(selectedLb);
        ctrl0.setTLB(selectedLb);
        eeTLB->mapTlb(selectedLb);
    }
    mio::TlbPageEntry* EeMipsCore::fetchTlbFromCop(u32* c0Regs) {
        u16 c0id{*reinterpret_cast<u16*>(c0Regs[0])};
        return &eeTLB->entries[c0id];
    }
    void EeMipsCore::handleException(u8 el, u32 exceptVec, u8 code) {
        ctrl0.cause.exCode = code & 0xd;
        const u8 savePcId{static_cast<u8>(el == 1 ? 14 : 30)};
        if (isABranch) {
            ctrl0.mtc0(savePcId, *eePc - 4);
        } else {
            ctrl0.mtc0(savePcId, *eePc);
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

