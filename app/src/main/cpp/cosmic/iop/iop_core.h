#pragma once
#include <common/types.h>
#include <mio/blocks.h>

#include <iop/iop_fuji.h>
#include <iop/iop_cop.h>
namespace cosmic::iop {
    struct IOPCache {
        u32 data;
        u32 tag;
        bool isValid;
    };

    class IOMipsCore {
    public:
        IOMipsCore(std::shared_ptr<mio::GlobalMemory>& mem);
        void resetIOP();
        void pulse(u32 cycles);
        u32 fetchByPC();

        void intByINTC(bool isInt);
        void handleException(u32 vec, u8 code);

        std::array<u32, 32> IOGPRs;
        std::array<IOPCache, 128> iCache;
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
        u32 lastPC,
            ioPc,
            cyclesToIO;
        IopCop cop;
        bool onBranch{false};

    private:
        u8* iopPrivateAddrSolver(u32 address);

        u8 irqSpawned;
        std::unique_ptr<IOPExecVE> interpreter;
    };
}

