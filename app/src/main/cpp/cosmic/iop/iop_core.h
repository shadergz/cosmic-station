#pragma once
#include <common/types.h>
#include <mio/blocks.h>

#include <iop/iop_fuji.h>
#include <iop/iop_cop.h>
namespace cosmic::iop {
    struct IoCache {
        u32 data;
        u32 tag;
        bool isValid;
    };

    class IoMipsCore {
    public:
        IoMipsCore(std::shared_ptr<mio::GlobalMemory>& mem);
        void resetIOP();
        void pulse(u32 cycles);
        u32 fetchByPC();

        void intByINTC(bool isInt);
        void handleException(u8 code);
        std::array<u32, 32> IOGPRs;
        std::array<IoCache, 128> iCache;
        u32 cacheCtrl;
        std::shared_ptr<mio::GlobalMemory> iopMem;

        template <typename T>
        T iopRead(u32 address) {
            // KSeg0
            if (address >= 0x80000000 && address < 0xa0000000)
                address -= 0x80000000;
            // KSeg1
            else if (address >= 0xa0000000 && address < 0xc0000000)
                address -= 0xa0000000;
            // KUSeg, KSeg2
            u8* readFrom{iopMem->iopUnalignedRead(address)};
            if (readFrom)
                return *reinterpret_cast<T*>(readFrom);
            return *reinterpret_cast<T*>(iopPrivateAddrSolver(address));
        }
        u32 hi, lo;
        u32 lastPc,
            ioPc,
            waitPc;
        i64 cyclesToIO;
        IopCop cop;
        bool onBranch{false};
        u8 branchDelay{};
        i32 mathDelay{};

        void takeBranchIf(bool take, i32 pcAddr);

        u8 irqSpawned;
    private:
        u8* iopPrivateAddrSolver(u32 address);

        std::unique_ptr<IopExecVE> interpreter;
    };
}

