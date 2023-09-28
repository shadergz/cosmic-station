#include <cstring>
#include <paper_debug.h>
#include <eeiv/mmu_tlb.h>

namespace zenith::eeiv {
    TLBCache::TLBCache(const std::shared_ptr<console::GlobalMemory>& glbRef)
        : physicalBlk(glbRef) {

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
            PaperRtAssert(kVTable < 1024 * 1024, "");
            kernelVTLB[kVTable] = choiceMemSrc(segmentPage & (0x20000000 - 1));

            if (segmentPage < 0xa0000000)
                tlbInfo[kVTable].ccMode0 = TLBCacheMode::Cached;
            else
                tlbInfo[kVTable].ccMode0 = TLBCacheMode::Uncached;
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
            mapAddress = physicalBlk->makeRealAddress(logicalA);
        } else if (logicalA >= 0x1fc00000 && logicalA < 0x20000000) {
            // Accessing the physical memory of the BIOS, not yet implemented, under construction
            mapAddress = physicalBlk->makeRealAddress(logicalA, true);
        }
        return mapAddress;
    }
}
