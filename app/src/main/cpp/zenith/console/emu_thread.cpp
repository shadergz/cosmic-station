#include <mutex>
#include <console/emu_thread.h>

namespace zenith::console {
    static std::mutex mlMutex;
    static std::condition_variable mlCond;
    std::atomic<bool> isRunning;

    void EmuThread::vmMain() {
        std::unique_lock<std::mutex> un(mlMutex);
        pthread_setname_np(pthread_self(), "VmMain");
        mlCond.wait(un, [](){ return isRunning.load(std::memory_order_consume); });
    }
    EmuThread::EmuThread() {
        vmt = std::thread(vmMain);
    }
    void EmuThread::runVM() {
        isRunning.store(true);
        mlCond.notify_one();
    }
}
