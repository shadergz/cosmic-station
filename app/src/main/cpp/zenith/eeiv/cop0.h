#pragma once

#include <types.h>

#include <eeiv/mmu_tlb.h>
#include <eeiv/high_fast_cache.h>
namespace zenith::eeiv {
    static constexpr u8 cop0RegsCount{32};

    enum KSU : u8 {
        kernel,
        supervisor,
        user
    };

    union Cop0Status {
        u32 rawStatus{};
        struct {
            u8 copUsable;
            // Set when a level 1 exception occurs
            bool exception;
            // Set when a level 2 exception occurs
            bool error;
            KSU mode;
        };
    };

    class EEMipsCore;

    class CoProcessor0 {
    public:
        static constexpr u8 countOfCacheLines{128};

        static constexpr auto invCacheLRF{static_cast<u32>(1 << 31)};

        CoProcessor0();
        CoProcessor0(CoProcessor0&&) = delete;
        CoProcessor0(CoProcessor0&) = delete;
        ~CoProcessor0();

#pragma pack(push, 4)
        struct {
            // The codenamed pRid register determines in the very early boot process for the BIOS
            // which processor it is currently running on, whether it's on the EE or the PSX
            Cop0Status status;
            u32 pRid;
        };
#pragma pack(pop)

        u8** mapVirtualTLB(std::shared_ptr<TLBCache>& tlb);
        void resetCoP();

        bool isCacheHit(u32 address, u8 lane);
        EECacheLine* viewLine(u32 address);
        u32 readCache32(u32 address);
        void fillCacheWay(u32 address, u32 tag);
        void loadCacheLine(u32 address, EEMipsCore& eeCore);

        EECacheLine* eeNearCache;
    private:
        u32 copGPRs[cop0RegsCount];
    };

    static_assert(offsetof(CoProcessor0, pRid) == sizeof(u32) * 1);
    static_assert(sizeof(u32) * cop0RegsCount == 128);
}

