#pragma once
#include <memory>

#include <common/types.h>
#include <mio/blocks.h>

// kuseg | 00000000h-7fffffffh | User, TLB-mapped
// kseg0 | 80000000h-9fffffffh | Kernel, directly-mapped, cached
// kseg1 | a0000000h-bfffffffh | Kernel, directly-mapped, uncached
namespace cosmic::mio {
    enum TLBCacheMode : u32 {
        Invalid = 0b00,
        Uncached = 0b10,
        Cached = 0b11,
        UncachedAccelerated = 0b111
    };

    struct TLBPageEntry {
        TLBCacheMode cacheMode[2];
        // Scratchpad. When set, the virtual mapping goes to scratchpad instead of main memory
        bool isSPad;
        u32 pfn[2];
        u32 dirty[2];
        u32 valid[2];
        u32 asid;
        u32 vpn2;
        u32 pageMask;
        u32 pageSize;
        u32 pageShift;

        bool isGlobal;
        bool modified;
    };
    struct TLBInfo {
        u32 cacheMode;
        bool isModified;
    };

    class TLBCache {
    public:
        TLBCache(std::shared_ptr<GlobalMemory>& global);
        ~TLBCache();

        u8** userVTLB{};
        u8** supervisorVTLB{};
        u8** kernelVTLB{};

        TLBInfo* tlbInfo{};
        std::array<TLBPageEntry, 48> entries;

        u8* choiceMemSrc(u32 logicalA);

        bool isCached(u32 address);
        void tlbChModified(u32 page, bool value);

        void mapTLB(TLBPageEntry& entry);
        void unmapTLB(TLBPageEntry& entry);
    private:
        std::shared_ptr<GlobalMemory> blocks;
    };

}
