#include <cstring>
#include <common/except.h>
#include <mio/mmu_tlb.h>
namespace zenith::mio {
    TLBCache::TLBCache(std::shared_ptr<link::GlobalMemory>& global)
        : block(global) {

        if (!userVTLB)
            userVTLB = new u8*[1024 * 1024];
        if (!supervisorVTLB)
            supervisorVTLB = new u8*[1024 * 1024];
        if (!kernelVTLB)
            kernelVTLB = new u8*[1024 * 1024];
        if (!tlbInfo)
            tlbInfo = new TLBPageEntry[1024 * 1024];

        std::memset(userVTLB, 0, sizeof(u8*) * 1024 * 1024);
        std::memset(supervisorVTLB, 0, sizeof(u8*) * 1024 * 1024);
        std::memset(kernelVTLB, 0, sizeof(u8*) * 1024 * 1024);
        std::memset(tlbInfo, 0, sizeof(TLBPageEntry) * 1024 * 1024);

        constexpr u32 kUnmapStart{0x80000000};
        constexpr u32 kUnmapEnd{0xc0000000};

        // Kernel page segments are not mapped in the TLB; we need to pass physical addresses
        // directly to the table entries
        for (auto segmentPage{kUnmapStart}; segmentPage != kUnmapEnd; segmentPage += 4096) {
            auto kVTable{segmentPage / 4096};
            if (kVTable >= 1024 * 1024) {
                throw MMUFail("Kernel TLB table {} is outside the specified range", kVTable);
            }

            kernelVTLB[kVTable] = choiceMemSrc(segmentPage & (0x20000000 - 1));
            if (segmentPage < 0xa0000000)
                tlbInfo[kVTable].cacheMode[0] = TLBCacheMode::Cached;
            else
                tlbInfo[kVTable].cacheMode[0] = TLBCacheMode::Uncached;
        }
    }

    TLBCache::~TLBCache() {
        delete[] userVTLB;
        delete[] supervisorVTLB;
        delete[] kernelVTLB;

        delete[] tlbInfo;
    }

    u8* TLBCache::choiceMemSrc(u32 logicalA) {
        u8* mapAddress{};
        [[likely]] if (logicalA < 0x10000000) {
            mapAddress = block->makeRealAddress(logicalA);
        } else if (logicalA >= 0x1fc00000 && logicalA < 0x20000000) {
            // Accessing the physical memory of the BIOS, not yet implemented, under construction
            mapAddress = block->makeRealAddress(logicalA, true);
        }
        return mapAddress;
    }

    void TLBCache::tlbChModified(u32 page, bool value) {
        if (page >= 1024 * 1024)
            throw MMUFail("Page {} is outside the range, TLB is missing for this page", page);
        tlbInfo[page].modified = value;
    }

    bool TLBCache::isCached(u32 address) {
        return tlbInfo[address / 4096].cacheMode[0] == TLBCacheMode::Cached;
    }
}
