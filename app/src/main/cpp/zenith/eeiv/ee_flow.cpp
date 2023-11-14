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

    void EEMipsCore::setTLBByIndex() {
        eeTLB->unmapTLB(eeTLB->entries[cop0.tlbIndex]);
        cop0.setTLB(eeTLB->entries[cop0.tlbIndex]);
        eeTLB->mapTLB(eeTLB->entries[cop0.tlbIndex]);
    }

    mio::TLBPageEntry* EEMipsCore::fetchTLBFromCop(u32* c0Regs) {
        u16 c0id{*reinterpret_cast<u16*>(c0Regs[0])};
        return &eeTLB->entries[c0id];
    }
    void EEMipsCore::handleException(u8 el, u32 exceptVec, u8 code) {
        cop0.cause.exCode = code & 0xd;
        const u8 savePCid{static_cast<u8>(el == 1 ? 14 : 30)};

        if (isABranch) {
            cop0.mtc0(savePCid, *eePC - 4);
        } else {
            cop0.mtc0(savePCid, *eePC);
        }

        if (savePCid == 14) {
            cop0.cause.bd = isABranch;
            cop0.status.exception = true;
        } else {
            cop0.cause.bd2 = isABranch;
            cop0.status.error = true;
        }

        if (cop0.status.bev || cop0.status.dev) {
            exceptVec |= 0xbfc00;
            exceptVec += 200;
        }

        isABranch = false;
        chPC(exceptVec);
    }
}

