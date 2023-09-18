#pragma once
#include <impl_types.h>

// kuseg | 00000000h-7fffffffh | User, TLB-mapped
namespace zenith::eeiv {
    enum TLBCacheMode : u32 {
        Invalid = 0b00,
        Uncached = 0b10,
        Cached = 0b11,
        UncachedAccelerated = 0b111

    };

    struct TLBPageEntry {
        u32 v0: 1;
        u32 d0: 1;
        TLBCacheMode c0: 3{TLBCacheMode::Invalid};
        u32 pfn0: 19;
        u8 hwReserved0;
        u32 v1: 1;
        u32 d1: 1;
        u32 c1: 1;
        u32 hwReserved1: 2;
        u32 pfn1: 19;
        u32 hwReserved2: 5;
        // S - Scratchpad. When set, the virtual mapping goes to scratchpad instead of main memory
        u32 s: 1;
        u32 aSID: 7;
        u32 hwReserved3: 4;
        u32 g: 2;
        u32 vpn2: 18;
        u32 hwReserved4: 12;
        u32 mask: 11;
    };

    class TLBCache {
    public:
        TLBCache();
        ~TLBCache();

        u8** m_userVTLB{};
        u8** m_supervisorVTLB{};
        u8** m_kernelVTLB{};

        TLBPageEntry* m_tlbInfo{};
    };

}
