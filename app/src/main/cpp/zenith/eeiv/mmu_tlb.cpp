#include <cstring>
#include <paper_assert.h>
#include <eeiv/mmu_tlb.h>

namespace zenith::eeiv {
    TLBCache::TLBCache(const std::shared_ptr<console::GlobalMemory>& glbRef)
        : m_physicalBlk(glbRef)
          {

        if (!m_userVTLB)
            m_userVTLB = new u8*[1024 * 1024];
        if (!m_supervisorVTLB)
            m_supervisorVTLB = new u8*[1024 * 1024];
        if (!m_kernelVTLB)
            m_kernelVTLB = new u8*[1024 * 1024];
        if (!m_tlbInfo)
            m_tlbInfo = new TLBPageEntry[1024 * 1024];

        std::memset(m_userVTLB, 0, sizeof(u8*) * 1024 * 1024);
        std::memset(m_supervisorVTLB, 0, sizeof(u8*) * 1024 * 1024);
        std::memset(m_kernelVTLB, 0, sizeof(u8*) * 1024 * 1024);
        std::memset(m_tlbInfo, 0, sizeof(TLBPageEntry) * 1024 * 1024);

        constexpr u32 kUnmapStart{0x80000000};
        constexpr u32 kUnmapEnd{0xc0000000};

        // Kernel page segments are not mapped in the TLB; we need to pass physical addresses
        // directly to the table entries
        // kseg0 | 80000000h-9fffffffh | Kernel, directly-mapped, cached
        // kseg1 | a0000000h-bfffffffh | Kernel, directly-mapped, uncached
        for (auto segmentPage{kUnmapStart}; segmentPage != kUnmapEnd; segmentPage += 4096) {
            auto kVTable{segmentPage / 4096};
            PaperRtAssert(kVTable < 1024 * 1024, "");
            m_kernelVTLB[kVTable] = choiceMemSrc(segmentPage & (0x20000000 - 1));

            if (segmentPage < 0xa0000000)
                m_tlbInfo[kVTable].c0 = TLBCacheMode::Cached;
            else
                m_tlbInfo[kVTable].c0 = TLBCacheMode::Uncached;
        }
    }

    TLBCache::~TLBCache() {
        delete[] m_userVTLB;
        delete[] m_supervisorVTLB;
        delete[] m_kernelVTLB;

        delete[] m_tlbInfo;
    }

    u8* TLBCache::choiceMemSrc(u32 logicalA) {
        u8* mapAddress{};
        [[likely]] if (logicalA < 0x10000000) {
            mapAddress = m_physicalBlk->makeRealAddress(logicalA);
        } else if (logicalA >= 0x1fc00000 && logicalA < 0x20000000) {
            // Accessing the physical memory of the BIOS, not yet implemented, under construction
            mapAddress = m_physicalBlk->makeRealAddress(logicalA, true);
        }
        return mapAddress;
    }
}
