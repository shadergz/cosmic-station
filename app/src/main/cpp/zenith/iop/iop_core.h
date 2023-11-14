#pragma once
#include <common/types.h>
#include <iop/iop_fuji.h>
#include <mio/blocks.h>

namespace zenith::iop {
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

        std::array<u32, 32> IOGPRs;
        std::array<IOPCache, 128> iCache;

        std::shared_ptr<mio::GlobalMemory> iopMem;

        template <typename T>
        T iopRead(u32 address) {
            return *reinterpret_cast<T*>(iopMem->iopUnalignedRead(address));
        }
    public:
        u32 lastPC,
            ioPc,
            cyclesToIO;

        u32 hi, lo;

        u8 irqSpawned;
        std::unique_ptr<IOPExecVE> interpreter;
    };
}

