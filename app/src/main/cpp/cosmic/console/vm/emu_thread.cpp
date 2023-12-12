// SPDX-short-identifier: MIT, Version N/A
// This file is protected by the MIT license (please refer to LICENSE.md before making any changes, copying, or redistributing this software)
#include <mutex>
#include <common/global.h>
#include <console/vm/emu_thread.h>

#include <console/vm/emu_vm.h>
#define SVR_CHANGE_MODE(ck, mode)\
    ck &= 0xff00 | mode
#define SVR_INVALIDATE(ck)\
    ck = (svrFinished << 8) & 0xff00
#define SVR_CREATE(ck) \
    ck = (svrRunning << 8) | svrMonitor3
namespace cosmic::console::vm {
    static std::mutex mlMutex{};
    static std::condition_variable mlCond{};

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
                    vm->sharedPipe->controller->pulse(busCycles);
                    vm->mpegDecoder->update();
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
            vm->hasFrame = true;
        }
    }

    void EmuThread::vmMain(std::shared_ptr<EmuShared> owner) {
        std::unique_lock<std::mutex> unique(mlMutex);
        pthread_setname_np(pthread_self(), "Vm.Emu");

        auto vm{owner->frame};
        mlCond.wait(unique, [owner](){ return (owner->check & 0xff) == svrMonitor2; });

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
            runFrameLoop(owner);
            // Todo: Just for testing purposes
            owner->isRunning = owner->isMonitoring = false;
            statusRunning = owner->isRunning;
            owner->executionCount++;
        } while (statusRunning);

        SVR_INVALIDATE(owner->check);
    }
    void EmuThread::updateValues(bool running, u8 isSuper) {
        std::scoped_lock<std::mutex> scoped(mlMutex);
        if (isSuper == 0x1 || isSuper == 0x3)
            shared->isMonitoring = running;
        if (isSuper == 0x2 || isSuper == 0x3)
            shared->isRunning = running;

    }
    void EmuThread::vmSupervisor(std::shared_ptr<EmuShared> owner) {
        pthread_setname_np(pthread_self(), "Vm.Monitor");
        std::reference_wrapper<std::atomic_bool> isAlive{owner->isRunning};
        SVR_CREATE(owner->check);
        for (; (owner->check >> 8) != svrFinished; ) {
            // The supervision thread will be executed every 95 milliseconds
            std::this_thread::sleep_for(std::chrono::nanoseconds(95'000));
            if (owner->check & svrMonitor3) {
                if (isAlive.get())
                    isAlive = owner->isMonitoring;
            } else if (owner->check & svrMonitor1) {
                if (isAlive.get())
                    continue;
                SVR_INVALIDATE(owner->check);
                isAlive = owner->isRunning;
            }
            [[likely]] if (isAlive.get()) {
                if (owner->check & svrMonitor3) {
                    std::scoped_lock<std::mutex> scope(mlMutex);
                    SVR_CHANGE_MODE(owner->check, svrMonitor2);
                    mlCond.notify_one();
                } else if (owner->check & svrMonitor2) {
                    // We're monitoring the EmuThread behavior
                    std::this_thread::yield();
                    SVR_CHANGE_MODE(owner->check, svrMonitor1);
                }
            }
        }
    }

    void EmuThread::switchVmPower(bool is) {
        if (is) {
            updateValues(is, 0x3);
        } else {
            updateValues(is, 0x1);
            updateValues(is, 0x2);
        }
    }
    EmuThread::EmuThread(EmuVM& vm) {
        shared = std::make_shared<EmuShared>();
        shared->frame = raw_reference<EmuVM>(vm);
    }
    void EmuThread::haltVM() {
        switchVmPower(false);
        if (vmt.joinable())
            vmt.join();
    }
    void EmuThread::runVM() {
        if (vmt.joinable())
            vmt.detach();
        vmt = std::thread(vmSupervisor, shared);
        switchVmPower(true);
        vmMain(shared);
        vmt.join();
    }
}
