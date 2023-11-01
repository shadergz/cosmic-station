#pragma once

#include <common/types.h>

#include <eeiv/mmu_tlb.h>
#include <eeiv/c0/high_fast_cache.h>

namespace zenith::eeiv {
    class EEMipsCore;
}

namespace zenith::eeiv::c0 {
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

    union Cop0Cause {
        u32 rawCause{};
        struct {
            bool timerIP;
        };
    };

    class CoProcessor0 {
    public:
        static constexpr u8 countOfCacheLines{128};

        static constexpr auto invCacheBit{static_cast<u32>(1 << 31)};

        CoProcessor0();
        CoProcessor0(CoProcessor0&&) = delete;
        CoProcessor0(CoProcessor0&) = delete;
        ~CoProcessor0();

        union alignas(4) {
            // The codenamed pRid register determines in the very early boot process for the BIOS
            // which processor it is currently running on, whether it's on the EE or the PSX
            struct {
                Cop0Status status;
                u32 count;
                u32 compare;
                u32 pRid;
                u32 perfCounter;
                Cop0Cause cause;
            };
            std::array<u32, cop0RegsCount> GPRs;
        };

        u8** mapVirtualTLB(std::shared_ptr<TLBCache>& virtTable);
        void resetCoP();
        void rectifyTimer(u32 pulseCycles);

        bool isCacheHit(u32 address, u8 lane);
        EECacheLine* viewLine(u32 address);
        u32 readCache(u32 address);
        void fillCacheWay(EECacheLine* line, u32 tag);
        void loadCacheLine(u32 address, EEMipsCore& eeCore);

        bool isIntEnabled();
    private:
        void incPerfByEvent(u32 mask, u32 cycles, u8 event01);
        EECacheLine* eeNearCache;
    };

    static_assert(offsetof(CoProcessor0, pRid) == sizeof(u32) * 3);
    static_assert(sizeof(u32) * cop0RegsCount == 128);
}

