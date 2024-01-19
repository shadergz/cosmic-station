#include <iostream>
#include <common/except.h>
#include <mio/mmu_tlb.h>
namespace cosmic::mio {
    TlbCache::TlbCache(std::shared_ptr<GlobalMemory>& global) :
        blocks(global) {
        std::memset(entries.data(), 0, sizeof(entries));
        if (!userVtlb) {
            userVtlb = new u8*[1024 * 1024];
        }
        std::memset(userVtlb, 0, sizeof(u8*) * 1024 * 1024);
        if (!supervisorVtlb) {
            supervisorVtlb = new u8*[1024 * 1024];
        }
        std::memset(supervisorVtlb, 0, sizeof(u8*) * 1024 * 1024);
        if (!kernelVtlb) {
            kernelVtlb = new u8*[1024 * 1024];
        }
        if (!tlbInfo) {
            tlbInfo = new TlbInfo[1024 * 1024];
        }
        std::memset(kernelVtlb, 0, sizeof(u8*) * 1024 * 1024);
        std::memset(tlbInfo, 0, sizeof(TlbInfo) * 1024 * 1024);

        constexpr u32 kUnmapStart{0x80000000};
        constexpr u32 kUnmapEnd{0xc0000000};
        // Kernel page segments are not mapped in the TLB; we need to pass physical addresses
        // directly to the table entries
        for (auto segmentPage{kUnmapStart}; segmentPage != kUnmapEnd; segmentPage += 4096) {
            auto kVTable{segmentPage / 4096};
            if (kVTable >= 1024 * 1024) {
                throw MioFail("Kernel TLB table {} is outside the specified range", kVTable);
            }

            kernelVtlb[kVTable] = choiceMemSrc(segmentPage & (0x20000000 - 1));
            if (segmentPage < 0xa0000000)
                tlbInfo[kVTable].cacheMode = TlbCacheMode::Cached;
            else
                tlbInfo[kVTable].cacheMode = TlbCacheMode::Uncached;
        }
    }

    TlbCache::~TlbCache() {
        delete[] userVtlb;
        delete[] supervisorVtlb;
        delete[] kernelVtlb;

        delete[] tlbInfo;
    }
    u8* TlbCache::choiceMemSrc(u32 logicalA) {
        u8* mapAddress{};
        [[likely]] if (logicalA < 0x10000000) {
            mapAddress = blocks->mapVirtAddress(logicalA);
        } else if (logicalA >= 0x1fc00000 && logicalA < 0x20000000) {
            // Accessing the physical memory of the BIOS, not yet implemented, under construction
            mapAddress = blocks->mapVirtAddress(logicalA, mio::BiosMemory);
        }
        return mapAddress;
    }

    void TlbCache::tlbChangeModified(u32 page, bool value) {
        if (page >= 1024 * 1024)
            throw MioFail("Page {} is outside the range, TLB is missing for this page", page);
        tlbInfo[page].isModified = value;
    }
    bool TlbCache::isCached(u32 address) {
        return tlbInfo[address / 4096].cacheMode == TlbCacheMode::Cached;
    }
}
