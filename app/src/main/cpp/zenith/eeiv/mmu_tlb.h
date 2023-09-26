#pragma once
#include <memory>
#include <impl_types.h>
#include <link/global_memory.h>

// kuseg | 00000000h-7fffffffh | User, TLB-mapped
// kseg0 | 80000000h-9fffffffh | Kernel, directly-mapped, cached
// kseg1 | a0000000h-bfffffffh | Kernel, directly-mapped, uncached
namespace zenith::eeiv {
    enum TLBCacheMode : u32 {
        Invalid = 0b00,
        Uncached = 0b10,
        Cached = 0b11,
        UncachedAccelerated = 0b111
    };

    struct TLBPageEntry {
        TLBCacheMode ccMode0{TLBCacheMode::Invalid};
        // Scratchpad. When set, the virtual mapping goes to scratchpad instead of main memory
        bool scratchpad;
    };

    class TLBCache {
    public:
        TLBCache(const std::shared_ptr<console::GlobalMemory>& glbRef);
        ~TLBCache();

        u8** m_userVTLB{};
        u8** m_supervisorVTLB{};
        u8** m_kernelVTLB{};

        TLBPageEntry* m_tlbInfo{};

        u8* choiceMemSrc(u32 logicalA);
    private:
        std::shared_ptr<console::GlobalMemory> m_physicalBlk;
    };

}
