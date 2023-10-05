#pragma once
#include <memory>
#include <types.h>
#include <link/glb_memory.h>

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
        bool modified;
    };

    class TLBCache {
    public:
        TLBCache(const std::shared_ptr<console::GlobalMemory>& glbRef);
        ~TLBCache();

        u8** userVTLB{};
        u8** supervisorVTLB{};
        u8** kernelVTLB{};

        TLBPageEntry* tlbInfo{};

        u8* choiceMemSrc(u32 logicalA);
        void tlbChModified(u32 page, bool value);
    private:
        std::shared_ptr<console::GlobalMemory> physicalBlk;
    };

}
