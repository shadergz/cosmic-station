// SPDX-short-identifier: MIT, Version N/A
// This file is protected by the MIT license (please refer to LICENSE.md before making any changes, copying, or redistributing this software)
#include <mutex>
#include <common/global.h>
#include <vm/emu_thread.h>

#include <vm/emu_vm.h>
namespace cosmic::vm {
    static std::mutex mlMutex{};
    static std::condition_variable mlCond{};
    void EmuThread::vmMain(std::shared_ptr<EmuShared> owner) {
        std::unique_lock<std::mutex> unique(mlMutex);

        pthread_setname_np(pthread_self(), "Vm.Emu");
        auto vm{owner->frame};
        mlCond.wait(unique, [owner](){ return (owner->check & 0xff) == svrMonitor2; });

        device->getStates()->addObserver(os::SchedulerAffinity, [&](JNIEnv* os) {
            bool state{owner->isRunning};
            if (owner->isRunning)
                owner->isRunning = false;
            switch (device->getStates()->schedAffinity.cachedState) {
            case Normal:
                // EE, GS, VUs
                vm->scheduler->affinity = EmotionEngine | GS << 4 | VUs << 8; break;
            case PrioritizeVectors:
                // VUs, EE, GS
                vm->scheduler->affinity = VUs | EmotionEngine << 4 | GS << 8; break;
            case GraphicsFirst:
                // GS, VUs, EE
                vm->scheduler->affinity = GS | VUs << 4 | EmotionEngine << 8; break;
            }
            owner->isRunning = state;
        });

        auto cyclesSched{vm->scheduler};
        if (!cyclesSched->affinity)
            cyclesSched->affinity = EmotionEngine | GS << 4 | VUs << 8;
        bool statusRunning;
        do {
            vm->hasFrame = false;
            runFrameLoop(owner);
            // Todo: Just for testing purposes
            if (owner->executionCount == 512)
                owner->isRunning = owner->isMonitoring = false;

            statusRunning = owner->isRunning;
            owner->executionCount++;
        } while (statusRunning);

        owner->check = (svrFinished << 8) & 0xff00;
    }
    void EmuThread::updateValues(bool running, u8 isSuper) {
        std::scoped_lock<std::mutex> scoped(mlMutex);
        if (isSuper == 0x3) {
            shared->isMonitoring = running;
            shared->isRunning = running;
        }
        if (isSuper == 0x1)
            shared->isMonitoring = running;
        else if (isSuper == 0x2)
            shared->isRunning = running;
    }

    void EmuThread::vmSupervisor(std::shared_ptr<EmuShared> owner) {
        pthread_setname_np(pthread_self(), "Vm.Monitor");
        std::reference_wrapper<std::atomic_bool> isAlive{owner->isRunning};

        owner->check = (svrRunning << 8) | svrMonitor3;

        for (; (owner->check >> 8) != svrFinished; ) {
            // The supervision thread will be executed every 95 milliseconds
            std::this_thread::sleep_for(std::chrono::nanoseconds(95'000));
            if (owner->check & svrMonitor3) {
                if (isAlive.get())
                    isAlive = owner->isMonitoring;
            } else if (owner->check & svrMonitor1) {
                if (isAlive.get())
                    continue;
                owner->check = (svrFinished << 8) & 0xff00;

                isAlive = owner->isRunning;
            }
            [[likely]] if (isAlive.get()) {
                if (owner->check & svrMonitor3) {
                    std::scoped_lock<std::mutex> scope(mlMutex);
                    owner->check &= 0xff00 | svrMonitor2;
                    mlCond.notify_one();
                } else if (owner->check & svrMonitor2) {
                    // We're monitoring the EmuThread behavior
                    std::this_thread::yield();

                    owner->check &= 0xff00 | svrMonitor1;
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
    EmuThread::EmuThread(EmuVm& vm) {
        shared = std::make_shared<EmuShared>();
        shared->frame = RawReference<EmuVm>(vm);
    }
    void EmuThread::haltVm() {
        switchVmPower(false);
        if (vmt.joinable())
            vmt.join();
    }
    void EmuThread::runVm() {
        if (vmt.joinable())
            vmt.detach();
        vmt = std::thread(vmSupervisor, shared);
        switchVmPower(true);
        vmMain(shared);
        vmt.join();
    }
}
