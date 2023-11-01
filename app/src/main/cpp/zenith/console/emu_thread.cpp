#include <mutex>
#include <console/emu_thread.h>
#include <common/global.h>

namespace zenith::console {
    static std::mutex mlMutex;
    static std::condition_variable mlCond;
    std::atomic<bool> isRunning;

    void EmuThread::vmMain() {
        std::unique_lock<std::mutex> un(mlMutex);
        pthread_setname_np(pthread_self(), "VmMain");
        mlCond.wait(un, [](){ return isRunning.load(std::memory_order_consume); });

        auto cyclesSched{zenithApp->vm->scheduler};
        while (isRunning) {
            u32 mipsCycles{cyclesSched->getNextCycles(Scheduler::Mips)};
            // u32 busCycles{cyclesSched->getNextCycles(Scheduler::Bus)};
            // u32 iopCycles{cyclesSched->getNextCycles(Scheduler::IOP)};

            cyclesSched->updateCycleCounts();

            zenithApp->vm->mips->pulse(mipsCycles);
        }
    }
    EmuThread::EmuThread() {
        vmt = std::thread(vmMain);
    }
    void EmuThread::runVM() {
        std::unique_lock<std::mutex> un(mlMutex);
        isRunning.store(true);
        mlCond.notify_one();
    }
}
