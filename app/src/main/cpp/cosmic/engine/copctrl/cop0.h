#pragma once

#include <common/types.h>

#include <mio/mmu_tlb.h>
#include <mio/dma_parallel.h>
namespace cosmic::engine {
    class EeMipsCore;
}
namespace cosmic::engine::copctrl {
    static constexpr u8 cop0RegsCount{32};
    struct alignas(8) CopCacheLine {
        std::array<u32, 2> tags;
        u32 data[2];
        bool lrf[2];
    };
    enum Ksu : u8 {
        Kernel,
        Supervisor,
        User
    };
     union Cop0Status {
        u64 ras{};
        struct {
            // Set when a level 1 exception occurs
            bool exception;
            // Set when a level 2 exception occurs
            bool error;
            Ksu mode : 3;
            bool masterIE;
            bool edi;

            // If set (bev or dev), level 1 exceptions go to "bootstrap" vectors in BFC00xxx
            bool bev;
            bool dev;
            u8 usable;
        };
    };

    union Cop0Cause {
        u32 rac{};
        struct {
            u8 exCode: 4;
            bool timerIP;
            // Set when a level 1 exception occurs in a delay slot
            bool bd;
            bool bd2;
        };
    };

    class CoProcessor0 {
    public:
        static constexpr u8 countOfCacheLines{128};
        static constexpr auto invCacheBit{static_cast<u32>(1 << 31)};

        CoProcessor0(std::shared_ptr<mio::DmaController>& ctrl);
        CoProcessor0(CoProcessor0&&) = delete;
        CoProcessor0(CoProcessor0&) = delete;
        ~CoProcessor0();

        union alignas(16) {
            // The codenamed pRid register determines in the very early boot process for the BIOS
            // which processor it is currently running on, whether it's on the EE or the PSX
            struct {
                u32 tlbIndex;
                u32 count;
                u32 compare;
                Cop0Status status;
                Cop0Cause cause;
                // PC backup value used when returning to an exception handler
                u32 ePC;
                u32 pRid;
                u32 perfCounter;
                u32 errorPC;
            };
            std::array<u32, cop0RegsCount> GPRs;
        };
        u32 perf0,
            perf1;

        u8** mapVirtualTlb(std::shared_ptr<mio::TlbCache>& virtTable);
        void resetCoP();
        void rectifyTimer(u32 pulseCycles);

        bool isCacheHit(u32 address, u8 lane);
        raw_reference<CopCacheLine> viewLine(u32 address);
        u32 readCache(u32 address);
        void fillCacheWay(raw_reference<CopCacheLine> line, u32 tag);
        void loadCacheLine(u32 address, raw_reference<EeMipsCore> eeCore);

        void invIndexed(u32 address);

        void loadFromGprToTlb(mio::TlbPageEntry& entry);
        void configureGlobalTlb(mio::TlbPageEntry& entry);

        void enableInt();
        void disableInt();

        bool isAHVector(u32 pcValue);
        bool haveAException();
        void mtc0(u8 reg, u32 code);
        u32 mfc0(u8 reg);

        bool getCondition();
        bool isIntEnabled();
    private:
        void incPerfByEvent(u32 mask, u32 cycles, u8 perfEv);
        CopCacheLine* iCacheLines;

        std::shared_ptr<mio::DmaController> dmac;
    };

    static_assert(sizeof(u32) * cop0RegsCount == 128);
}

