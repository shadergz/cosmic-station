// SPDX-short-identifier: MIT, Version N/A
// This file is protected by the MIT license (please refer to LICENSE.md before making any changes, copying, or redistributing this software)
#include <mutex>
#include <common/global.h>
#include <console/vm/emu_thread.h>

#include <console/vm/emu_vm.h>
namespace cosmic::console::vm {
    static std::mutex mlMutex;
    static std::condition_variable mlCond;
    std::atomic<bool> isRunning;

    void EmuThread::vmMain(EmuVM& vm) {
        std::unique_lock<std::mutex> un(mlMutex);
        pthread_setname_np(pthread_self(), "VM.Main");
        mlCond.wait(un, [](){ return isRunning.load(std::memory_order_consume); });

        device->getStates()->schedAffinity.observer = [&vm]() {
            bool state{isRunning};
            if (isRunning)
                isRunning = false;
            switch (device->getStates()->schedAffinity.cachedState) {
            case Normal: // EE, GS, VUs
                vm.scheduler->affinity = EmotionEngine | GS << 4 | VUs << 8; break;
            case PrioritizeVectors: // VUs, EE, GS
                vm.scheduler->affinity = VUs | EmotionEngine << 4 | GS << 8; break;
            case GraphicsFirst: // GS, VUs, EE
                vm.scheduler->affinity = GS | VUs << 4 | EmotionEngine << 8; break;
            }
            isRunning = state;
        };

        auto cyclesSched{vm.scheduler};
        u64 executionCount{};
        while (isRunning) {
            u32 mipsCycles{cyclesSched->getNextCycles(Scheduler::Mips)};
            u32 busCycles{cyclesSched->getNextCycles(Scheduler::Bus)};
            u32 iopCycles{cyclesSched->getNextCycles(Scheduler::IOP)};
            cyclesSched->updateCyclesCount();

            if (!cyclesSched->affinity)
                cyclesSched->affinity = EmotionEngine | GS << 4 | VUs << 8;

            for (u8 shift{}; shift < 3; shift++) {
                switch (cyclesSched->affinity >> (shift * 4) & 0xf) {
                case EmotionEngine:
                    vm.mips->pulse(mipsCycles);
                    vm.iop->pulse(iopCycles);
                    // DMAC runs in parallel, which could be optimized (and will be early next year)
                    vm.memCtrl->pulse(busCycles);
                    break;
                case GS:
                    break;
                case VUs:
                    // VUs can run in parallel with EE...
                    for (u8 runVifs{}; runVifs < 2; runVifs++)
                        vm.vu01->vifs[runVifs].update(busCycles);
                    vm.vu01->vpu0Cop2.pulse(mipsCycles);
                    vm.vu01->vpu1DLO.pulse(mipsCycles);
                    break;
                }
            }
            cyclesSched->runEvents();
            if (executionCount == 3)
                isRunning.store(false);
            executionCount++;
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
