#pragma once
#include <common/types.h>
#include <mio/mem_pipe.h>

#include <iop/iop_info.h>
#include <iop/iop_cop.h>
namespace cosmic::iop {
    struct IoCache {
        u32 data;
        u32 tag;
        bool isValid;
    };

    class IoMipsCore {
    public:
        IoMipsCore(std::shared_ptr<mio::MemoryPipe>& pipe);
        void resetIOP();
        void pulse(u32 cycles);
        u32 fetchByPC();

        void intByIntC(bool isInt);
        void handleException(u8 code);
        std::array<u32, 32> IoGPRs;
        std::array<IoCache, 128> instCache;
        u32 cacheCtrl;
        std::shared_ptr<mio::MemoryPipe> iopMem;

        template <typename T>
        T iopRead(u32 address) {
            // KSeg0
            if (address >= 0x80000000 && address < 0xa0000000)
                address -= 0x80000000;
            // KSeg1
            else if (address >= 0xa0000000 && address < 0xc0000000)
                address -= 0xa0000000;
            // KUSeg, KSeg2
            mio::VirtualPointer readFrom{
                iopMem->controller->mapped->makeRealAddress(address, mio::IopMemory)};
            if (readFrom)
                return readFrom.read<T*>();
            return *reinterpret_cast<T*>(iopPrivateAddrSolver(address));
        }
        u32 hi, lo;
        u32 lastPc,
            ioPc,
            waitPc;
        i64 cyclesToIo;
        IopCop cop;
        bool onBranch{false};
        u8 branchDelay{};
        i32 mathDelay{};

        void takeBranchIf(bool take, i32 pcAddr);
        u8 irqSpawned;
    private:
        u8* iopPrivateAddrSolver(u32 address);

        std::unique_ptr<IopExecVe> interpreter;
    };
}

