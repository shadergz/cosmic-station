#include <mutex>
#include <console/emu_thread.h>

#include <console/emu_vm.h>
namespace zenith::console {
    static std::mutex mlMutex;
    static std::condition_variable mlCond;
    std::atomic<bool> isRunning;

    void EmuThread::vmMain(EmuVM& vm) {
        std::unique_lock<std::mutex> un(mlMutex);
        pthread_setname_np(pthread_self(), "VM.Main");
        mlCond.wait(un, [](){ return isRunning.load(std::memory_order_consume); });

        auto cyclesSched{vm.scheduler};
        while (isRunning) {
            u32 mipsCycles{cyclesSched->getNextCycles(Scheduler::Mips)};
            u32 busCycles{cyclesSched->getNextCycles(Scheduler::Bus)};
            u32 iopCycles{cyclesSched->getNextCycles(Scheduler::IOP)};
            cyclesSched->updateCyclesCount();

            vm.mips->pulse(mipsCycles);
            vm.iop->pulse(iopCycles);

            // DMAC runs in parallel, which could be optimized (and will be early next year)
            vm.mips->dmac.pulse(busCycles);

            cyclesSched->runEvents();
            isRunning.store(false);
        }
    }
    EmuThread::EmuThread(EmuVM& vm) {
        vmt = std::thread(vmMain, std::ref(vm));
    }
    void EmuThread::runVM() {
        std::unique_lock<std::mutex> un(mlMutex);
        isRunning.store(true);
        mlCond.notify_one();
    }
}
