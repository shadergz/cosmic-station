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

    bool CoProcessor0::isAHVector(u32 pcValue) {
        if (status.exception) {
            switch (pcValue) {
            // TLB Refill
            case 0x80000000:
            case 0xbfc00200:
            // All others
            case 0x80000180:
            case 0xbfc00380:
            // Interrupt
            case 0x80000200:
            case 0xbfc00400:
                return true;
            }
        } else if (status.error) {
            switch (pcValue) {
            // Reset/NMI
            case 0xbfc00000:
            // Perf. Counter
            case 0x80000080:
            case 0xbfc00280:
            // Debug
            case 0x80000100:
            case 0xbfc00300:
                return true;
            }
        }

        return false;
    }
    bool CoProcessor0::haveAException() {
        return status.exception || status.error;
    }

    void CoProcessor0::mtc0(u8 reg, u32 code) {
        switch (reg) {
        case 14: // $14: EPC
            if (isAHVector(code) && haveAException()) {
                ePC = code;
            }
        case 30:
            if (isAHVector(code) && haveAException()) {
                errorPC = code;
            }
        }
    }
}