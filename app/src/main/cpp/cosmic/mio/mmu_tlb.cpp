#include <iostream>
#include <common/except.h>
#include <mio/mmu_tlb.h>
namespace cosmic::mio {
    TlbCache::TlbCache(std::shared_ptr<GlobalMemory>& global) :
            blocks(global) {
        std::memset(entries.data(), 0, sizeof(entries));
        virtArea.resize(1024 * 1024 * 3);
        tlbInfo.resize(1024 * 1024);

        userVirt = &virtArea[0];
        supervisorVirt = &virtArea[1024 * 1024];
        kernelVirt = &virtArea[1024 * 1024 * 2];

        virtArea.clear();
        tlbInfo.clear();
        constexpr u32 unmapStart{0x80000000};
        constexpr u32 unmapEnd{0xc0000000};

        // Kernel page segments are not mapped in the TLB; we need to pass physical addresses
        // directly to the table entries
        for (auto segmentPage{unmapStart}; segmentPage != unmapEnd; segmentPage += 4096) {
            auto table{segmentPage / 4096};
            if (table >= 1024 * 1024) {
                throw MioErr("Kernel TLB table {} is outside the specified range", table);
            }
            kernelVirt[table] = choiceMemSrc(segmentPage & (0x20000000 - 1));
            tlbInfo[table].cacheMode = TlbCacheMode::Uncached;

            if (segmentPage < 0xa0000000) {
                tlbInfo[table].cacheMode = TlbCacheMode::Cached;
            }
        }
    }
    TlbCache::~TlbCache() {
        // delete[] userVirt;
        // delete[] supervisorVirt;
        // delete[] kernelVirt;

        std::vector<TlbInfo> infoEmpty;
        tlbInfo.swap(infoEmpty);
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
        if (page >= 1024 * 1024) {
            throw MioErr("Page {} is outside the range, TLB is missing for this page", page);
        }
        tlbInfo[page].isModified = value;
    }
    bool TlbCache::isCached(u32 address) {
        return tlbInfo[address / 4096].cacheMode == TlbCacheMode::Cached;
    }
}
