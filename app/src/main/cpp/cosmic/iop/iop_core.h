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
        u32 fetchByPc();

        void intByIntC(bool isInt);
        void handleException(u8 code);
        u32 incPc();
        std::array<u32, 32> IoGPRs;
        std::array<IoCache, 128> instCache;
        u32 cacheCtrl;
        std::shared_ptr<mio::MemoryPipe> iopMem;

        u32 translateAddr(u32 address);
        bool isPcUncached(u32 pc);
        bool isRoRegion(u32 address);
        template <typename T>
        T pipeRead(u32 address) {
            return iopMem->solveGlobal(address & 0x1fffffff, mio::IopDev).as<T>();
        }

        template <typename T>
        T iopRead(u32 address) {
            address = translateAddr(address);
            if (isRoRegion(address))
                return *pipeRead<u8*>(address);
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

