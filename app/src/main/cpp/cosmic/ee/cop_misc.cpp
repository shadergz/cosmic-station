#include <ee/cop0.h>
#include <mio/mmu_tlb.h>
namespace cosmic::ee {
    // Due to the peculiarities of the implementation, the calling function of configureGlobalTlb
    // must map and unmap the TLB on its own
    void CtrlCop::configureGlobalTlb(mio::TlbPageEntry& entry) {
        entry.isSPad = GPRs[2] & static_cast<u32>(1 << 31);
        entry.pageMask = (GPRs[5] >> 13) & 0xffff;

        switch (entry.pageMask) {
        case 0x000:
            entry.pageSize = 1024 * 4; entry.pageShift = 0; break;
        case 0x003:
            entry.pageSize = 1024 * 16; entry.pageShift = 2; break;
        case 0x00f:
            entry.pageMask = 1024 * 16 * 4; entry.pageShift = 4; break;
        case 0x03f:
            entry.pageSize = 1024 * 256; entry.pageShift = 6; break;
        case 0x0ff:
            entry.pageSize = 1024 * 256 * 4; entry.pageShift = 8; break;
        case 0x3ff:
            entry.pageSize = 1024 * 256 * 16; entry.pageShift = 10; break;
        case 0xfff:
            entry.pageSize = 1024 * 256 * 32; entry.pageShift = 12; break;
        }

        // EntryHi
        entry.asid = GPRs[10] & 0xff;
        entry.vpn2 = GPRs[10] >> 0xd;

        entry.isGlobal = true;
        // PFN0 - Even page frame number
        // PFN1 - Odd page frame number
        // Scratchpad. When set, the virtual mapping goes to scratchpad instead of main memory
        // ASID - Address Space ID

        for (u8 rights{}; rights < 2; rights++) {
            u32 eLow{GPRs[rights + 2]};
            entry.isGlobal &= eLow & 1;

            entry.valid[rights] = (eLow >> 1) & 1;
            entry.dirty[rights] = (eLow >> 2) & 1;

            entry.cacheMode[rights] = static_cast<mio::TlbCacheMode>((eLow >> 3) & 0x7);
            entry.pfn[rights] = (eLow >> 6) & 0xfffff;
        }
        // VPN2 - Virtual page number / 2
        // Even pages have a VPN of (VPN2 * 2) and odd pages have a VPN of (VPN2 * 2) + 1
        u32 virtWorld{((entry.vpn2 * 2) >> entry.pageShift) * entry.pageSize};
        u32 realWorld{};

        if (entry.valid[0]) {
            realWorld = (entry.pfn[0] >> entry.pageShift) * entry.pageSize;
        } else if (entry.valid[1]) {
            realWorld = (entry.pfn[1] >> entry.pageShift) * entry.pageSize;
        }

        if (virtWorld == realWorld) {
            // throw Cop0Err("It is not possible to map physical addresses to virtual ones if they are the same");
        }
    }
    void CtrlCop::loadFromGprToTlb(mio::TlbPageEntry& entry) {
        // PageMask 000h=4 KB/FFFh=16 MB
        GPRs[5] = (entry.pageMask >> 13) & 0x0fff;
        // EntryHi (VPN | ASID) & ~PageMask
        GPRs[10] = ((entry.vpn2 << 13) | entry.asid) & ~GPRs[5];
        // EntryLo0
        GPRs[2] = (static_cast<u32>(entry.isSPad << 31)) |
            (entry.pfn[1] << 6) | (entry.cacheMode[1] << 3) |
            (entry.dirty[1] << 2) | (entry.valid[1] << 1) | entry.isGlobal;
        // EntryLo1
        GPRs[3] = (entry.pfn[0] << 6) | (entry.cacheMode[0] << 3) |
            (entry.dirty[0] << 2) | (entry.valid[0] << 1) | entry.isGlobal;
    }
#define CHECK_INT_FLAGS(c0St)\
    c0St.edi || c0St.mode == 0 || c0St.exception || c0St.error

    void CtrlCop::enableInt() {
        if (CHECK_INT_FLAGS(status))
            status.masterIe = true;
    }
    void CtrlCop::disableInt() {
        if (CHECK_INT_FLAGS(status))
            status.masterIe = false;
    }

    bool CtrlCop::isAHVector(u32 pcValue) {
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
    bool CtrlCop::haveAException() {
        return status.exception || status.error;
    }
    u32 CtrlCop::mfc0(u8 reg) {
#define STATUS_CAST(value) static_cast<u32>(value)
        u32 solved{};
        switch (reg) {
        case 12:
            solved |= STATUS_CAST(status.exception << 1);
            solved |= STATUS_CAST(status.bev << 22);
            break;
        case 14: solved = ePc; break;
        case 15: solved = pRid; break;
        case 30: solved = errorPc; break;
        default:
            if (reg >= GPRs.size()) {
            }
            solved = GPRs[reg];
        }
        return solved;
    }
    void CtrlCop::mtc0(u8 reg, u32 code) {
        bool inAnException{};
        bool inError{};

        switch (reg) {
        case 0:
            GPRs[0] = code;
            redoTlbMapping(); break;
        case 14: // $14: EPC
            inError = true; break;
        case 30:
            inAnException = true; break;
        default:
            GPRs[reg] = code; break;
        }

        if (inError &&
            isAHVector(code) &&
            haveAException()) {
            ePc = code;
        }
        if (inAnException && isAHVector(code) && haveAException()) {
            errorPc = code;
        }
    }
}