#include <eeiv/c0/cop0.h>
#include <mio/mmu_tlb.h>

namespace zenith::eeiv::c0 {
    void CoProcessor0::loadTlbValues(mio::TLBPageEntry& entry) {
        // PageMask
        GPRs[5] = entry.pageMask << 13;
        // EntryHi (VPN | ASID) & ~PageMask
        GPRs[10] = ((entry.vpn2 << 13) | entry.asid) & ~GPRs[5];
        // EntryLo0
        GPRs[2] = (static_cast<u32>(entry.isSPad << 31)) | (entry.pfn[1] << 6) | (entry.cacheMode[1] << 3) | (entry.dirty[1] << 2) | (entry.valid[1] << 1) | entry.isGlobal;
        // EntryLo1
        GPRs[3] = (entry.pfn[0] << 6) | (entry.cacheMode[0] << 3) | (entry.dirty[0] << 2) | (entry.valid[0] << 1) | entry.isGlobal;
    }

#define CheckIntFlags(c0St)\
    c0St.edi || c0St.mode == 0 || c0St.exception || c0St.error

    void CoProcessor0::enableInt() {
        if (CheckIntFlags(status))
            status.masterIE = true;
    }
    void CoProcessor0::disableInt() {
        if (CheckIntFlags(status))
            status.masterIE = false;
    }
}