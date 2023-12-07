// SPDX-short-identifier: MIT, Version N/A
// This file is protected by the MIT license (please refer to LICENSE.md before making any changes, copying, or redistributing this software)
#include <mutex>
#include <common/global.h>
#include <console/vm/emu_thread.h>

#include <console/vm/emu_vm.h>
namespace cosmic::console::vm {
    static std::mutex mlMutex;
    static std::condition_variable mlCond;

    void EmuThread::runFrameLoop(std::shared_ptr<EmuShared> owner) {
        auto vm{owner->frame};
        auto sched{vm->scheduler};
        while (!vm->hasFrame) {
            u32 mipsCycles{sched->getNextCycles(Scheduler::Mips)};
            u32 busCycles{sched->getNextCycles(Scheduler::Bus)};
            u32 iopCycles{sched->getNextCycles(Scheduler::IOP)};
            sched->updateCyclesCount();

            for (u8 shift{}; shift < 3; shift++) {
                switch (sched->affinity >> (shift * 4) & 0xf) {
                case EmotionEngine:
                    vm->mips->pulse(mipsCycles);
                    vm->iop->pulse(iopCycles);
                    // DMAC runs in parallel, which could be optimized (and will be early next year)
                    vm->memCtrl->pulse(busCycles);
                    break;
                case GS:
                    break;
                case VUs:
                    // VUs can run in parallel with EE...
                    for (u8 runVifs{}; runVifs < 2; runVifs++)
                        vm->vu01->vifs[runVifs].update(busCycles);
                    vm->vu01->vpu0Cop2.pulse(mipsCycles);
                    vm->vu01->vpu1DLO.pulse(mipsCycles);
                    break;
                }
            }
            sched->runEvents();
            // Todo: Just for testing purposes
            break;
        }
    }

    void EmuThread::vmMain(std::shared_ptr<EmuShared> owner) {
        std::unique_lock<std::mutex> unique(mlMutex);
        pthread_setname_np(pthread_self(), "Vm.Emu");

        auto vm{owner->frame};
        mlCond.wait(unique, [owner](){ return owner->isRunning.load(std::memory_order_consume); });

        device->getStates()->schedAffinity.observer = [&]() {
            bool state{owner->isRunning};
            if (owner->isRunning)
                owner->isRunning = false;
            switch (device->getStates()->schedAffinity.cachedState) {
            case Normal: // EE, GS, VUs
                vm->scheduler->affinity = EmotionEngine | GS << 4 | VUs << 8; break;
            case PrioritizeVectors: // VUs, EE, GS
                vm->scheduler->affinity = VUs | EmotionEngine << 4 | GS << 8; break;
            case GraphicsFirst: // GS, VUs, EE
                vm->scheduler->affinity = GS | VUs << 4 | EmotionEngine << 8; break;
            }
            owner->isRunning = state;
        };

        auto cyclesSched{vm->scheduler};
        if (!cyclesSched->affinity)
            cyclesSched->affinity = EmotionEngine | GS << 4 | VUs << 8;
        bool statusRunning;
        do {
            std::scoped_lock<std::mutex> scope(mlMutex);
            runFrameLoop(owner);
            statusRunning = owner->isRunning;
            owner->executionCount++;
        } while (statusRunning);
    }
    void EmuThread::vmSupervisor(std::shared_ptr<EmuShared> owner) {
        constexpr u8 svrFinished{0x85};
        constexpr u8 svrRunning{0x80};
        constexpr u8 svrMonitor1{0x10}; // Needs a check
        constexpr u8 svrMonitor2{0x20}; // Run an update
        constexpr u8 svrMonitor3{0x30}; // Initial state
        std::optional<std::atomic<bool>> isRun{&owner->isRunning};
        u16 check{svrRunning << 8 & svrMonitor3};
        for (; (check >> 8) != svrFinished; ) {
            if (check & svrMonitor1 || check & svrMonitor3) {
                std::scoped_lock<std::mutex> scope(mlMutex);
                if (check & svrMonitor3) {
                    if (*isRun) {
                        isRun = &owner->isMonitoring;
                    }
                } else if (check & svrMonitor1 && !isRun) {
                    check = svrFinished;
                    isRun.reset();
                }
            }
            if (!*isRun)
                continue;
            [[unlikely]] if (check & svrMonitor3) {
                std::unique_lock<std::mutex> advise{mlMutex};
                mlCond.notify_one();
                check = svrMonitor2;
            } else if (check & svrMonitor2) {
                std::this_thread::sleep_for(std::chrono::nanoseconds(5'000));
            }
        }
    }
    void EmuThread::switchVmPower(bool is) {
        std::scoped_lock<std::mutex> un(mlMutex);
        shared->isRunning.store(is);
        shared->isMonitoring.store(is);

        if (!is)
            vmt.join();
    }

    EmuThread::EmuThread(EmuVM& vm) {
        shared = std::make_shared<EmuShared>();
        shared->frame = raw_reference<EmuVM>(vm);
    }
    void EmuThread::haltVM() {
        switchVmPower(false);
    }
    void EmuThread::runVM() {
        vmt = std::thread(vmSupervisor, shared);
        switchVmPower(true);
        vmMain(shared);
    }
}
