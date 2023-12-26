#include <common/global.h>
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
            incPc();
    }
    void EeMipsCore::updateTlb() {
        tlbMap = ctrl0.mapVirtualTlb(eeTlb);
    }
    void EeMipsCore::setTlbByIndex() {
        auto selectedLb{std::ref(eeTlb->entries[ctrl0.tlbIndex])};

        eeTlb->unmapTlb(selectedLb);
        ctrl0.configureGlobalTlb(selectedLb);
        eeTlb->mapTlb(selectedLb);
    }
    raw_reference<mio::TlbPageEntry> EeMipsCore::fetchTlbFromCop(u32* c0Regs) {
        u16 c0id{*reinterpret_cast<u16*>(c0Regs[0])};
        return eeTlb->entries[c0id];
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
        delaySlot = 0;
        chPc(exceptVec);
        tlbMap = ctrl0.mapVirtualTlb(eeTlb);
    }

    void EeMipsCore::printStates() {
        fmt::memory_buffer states;
        fmt::format_to(back_inserter(states), "PC: {:#x}\n", eePc.pcValue);

        for (u32 ir{}; ir < 32; ir++) {
            fmt::format_to(back_inserter(states), "EE-GPR {}: dw0: {:#x}, dw1: {:#x}\n",
                gprsId[ir], GPRs[ir].dw[0], GPRs[ir].dw[1]);
        }
        fmt::format_to(back_inserter(states), "LO: {:#x}\n", mulDivStorage[0]);
        fmt::format_to(back_inserter(states), "HI: {:#x}\n", mulDivStorage[1]);
        static const std::array<std::string, 3> ksu{"Kernel", "Super", "User"};
        fmt::format_to(back_inserter(states), "KSU: {}\n", ksu[ctrl0.status.mode]);

        for (u32 cg{}; cg < 32; cg++)
            fmt::format_to(back_inserter(states), "EE-COP0: ID: {:#x}, Value {:#x}\n", cg, ctrl0.GPRs[cg]);
        for (u32 fg{}; fg < 32; fg++)
            fmt::format_to(back_inserter(states), "EE-COP1: ID: {:#x}, Value {:f}\n", fg, fpu1.fprRegs[fg].decimal);

        userLog->info("{}", states.data());
    }
    i64 EeMipsCore::getHtzCycles(bool total) const {
        return total ? ctrl0.count : wasteCycles;
    }
}

