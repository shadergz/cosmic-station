#pragma once
#include <memory>

#include <common/types.h>
#include <mio/blocks.h>

// kuseg | 00000000h-7fffffffh | User, TLB-mapped

// kseg0 | 80000000h-9fffffffh | Kernel, directly-mapped, cached

// kseg1 | a0000000h-bfffffffh | Kernel, directly-mapped, uncached

namespace cosmic::mio {
    enum TlbCacheMode : u32 {
        Invalid = 0b00,
        Uncached = 0b10,
        Cached = 0b11,
        UncachedAccelerated = 0b111
    };

    struct TlbPageEntry {
        std::array<TlbCacheMode, 2> cacheMode;
        // Scratchpad: When set, the virtual mapping goes to scratchpad instead of main memory
        bool isSPad;
        std::array<u32, 2> pfn;
        std::array<u32, 2> dirty;
        std::array<u32, 2> valid;
        u32 asid;
        u32 vpn2;
        u32 pageMask;
        u32 pageSize;
        u32 pageShift;

        bool isGlobal;
        bool modified;
    };
    struct TlbInfo {
        u32 cacheMode;
        bool isModified;
    };

    class TlbCache {
    public:
        TlbCache(std::shared_ptr<GlobalMemory>& global);
        ~TlbCache();
        u8** userVirt{};
        u8** supervisorVirt{};
        u8** kernelVirt{};

        std::array<TlbPageEntry, 48> entries;
        std::vector<u8*> virtArea;
        std::vector<TlbInfo> tlbInfo;

        u8* choiceMemSrc(u32 logicalA);

        bool isCached(u32 address);
        void tlbChangeModified(u32 page, bool value);

        void mapTlb(TlbPageEntry& entry);
        void unmapTlb(TlbPageEntry& entry);
    private:
        std::shared_ptr<GlobalMemory> blocks;
    };

}
