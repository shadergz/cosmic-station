#pragma once

#include <common/types.h>

#include <mio/mmu_tlb.h>
#include <mio/dma_ctrl.h>
namespace cosmic::engine {
    class EeMipsCore;
}
namespace cosmic::engine::copctrl {
    static constexpr u8 cop0RegsCount{32};
    class alignas(16) CopCacheLine {
    public:
        CopCacheLine() {

        }
        std::array<u32, 2> tags;
        bool lrf[2];

        struct CacheWay {
            u32 u[16];
            u64 large[8];
            os::vec vec[4]{};
        };
        std::array<CacheWay, 2> ec;
    };
    enum CacheMode {
        Instruction,
        Data
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
            bool masterIe;
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
            bool timerIp;
            // Set when a level 1 exception occurs in a delay slot
            bool bd;
            bool bd2;
        };
    };

    class CtrlCop {
    public:
        static constexpr u8 countOfCacheLines{128};
        static constexpr u32 validBit{static_cast<u32>(1 << 31)};
        static constexpr u32 dirtyBit{static_cast<u32>(1 << 31)};

        CtrlCop(std::shared_ptr<mio::DmaController>& ctrl);
        CtrlCop(CtrlCop&&) = delete;
        CtrlCop(CtrlCop&) = delete;
        ~CtrlCop();

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
        // Current virtual table being used by the processor
        u8** virtMap{};
        std::shared_ptr<mio::TlbCache> virtCache;

        void redoTlbMapping();
        void resetCoP();
        void rectifyTimer(u32 pulseCycles);

        bool isCacheHit(u32 address, u8 lane, CacheMode mode = Instruction);
        os::vec readCache(u32 address, CacheMode mode = Instruction);
        void assignFlushedCache(CopCacheLine& eec, u32 tag, CacheMode mode = Instruction);
        void loadCacheLine(u32 address, EeMipsCore& core, CacheMode mode = Instruction);
        u16 getCachePfn(u32 address, CacheMode mode = Instruction);

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
        RawReference<CopCacheLine> getCache(u32 mem, bool write, CacheMode mode = Instruction);

        std::array<CopCacheLine, 128> inCache;
        std::array<CopCacheLine, 64> dataCache;

        std::shared_ptr<mio::DmaController> dmac;
    };

    static_assert(sizeof(u32) * cop0RegsCount == 128);
}

