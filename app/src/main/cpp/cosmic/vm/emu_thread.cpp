// SPDX-short-identifier: MIT, Version N/A
// This file is protected by the MIT license (please refer to LICENSE.md before making any changes, copying, or redistributing this software)
#include <mutex>
#include <common/global.h>
#include <vm/emu_thread.h>

#include <vm/emu_vm.h>
namespace cosmic::vm {

    static std::mutex mlMutex{};

    static std::condition_variable mlCond{};
    void EmuThread::vmMain(std::shared_ptr<SharedVm>& svm) {
        std::unique_lock<std::mutex> unique(mlMutex);
        pthread_setname_np(pthread_self(), "Vm.Emu");
        mlCond.wait(unique, [&](){ return svm->getMonitor(0xff) == svrMonitor2; });
        svm->vm->status.set(IsMonitoring, true);

        device->getStates()->addObserver(os::SchedulerAffinity, [&](JNIEnv* os) {
            bool state{svm->vm->status.get(IsRunning)};
            if (state)
                svm->vm->status.set(IsRunning, false);
            switch (device->getStates()->schedAffinity.cachedState) {
            case Normal:
                // EE, GS, VUs
                svm->vm->scheduler->affinity = EmotionEngine | GS << 4 | VUs << 8; break;
            case PrioritizeVectors:
                // VUs, EE, GS
                svm->vm->scheduler->affinity = VUs | EmotionEngine << 4 | GS << 8; break;
            case GraphicsFirst:
                // GS, VUs, EE
                svm->vm->scheduler->affinity = GS | VUs << 4 | EmotionEngine << 8; break;
            }
            svm->vm->status.set(IsRunning, state);
        });

        auto cyclesSched{svm->vm->scheduler};
        if (!cyclesSched->affinity)
            cyclesSched->affinity = EmotionEngine | GS << 4 | VUs << 8;
        bool statusRunning;
        svm->vm->status.set(IsRunning, true);
        do {
            svm->vm->status.set(HasFrame, false);
            runFrameLoop(svm->vm);
            // Todo: Just for testing purposes
            if (svm->vm->status.getExecutionCount() == 512) {
                svm->vm->status.set(IsRunning, false);
                svm->vm->status.set(IsMonitoring, false);
            }

            statusRunning = svm->vm->status.get(IsRunning);
        } while (statusRunning);

        svm->setMonitor((svrFinished << 8) & 0xff00);
    }
    void EmuThread::updateValues(std::shared_ptr<SharedVm>& svm, bool running, u8 isSuper) {
        std::scoped_lock<std::mutex> scoped(mlMutex);
        if (isSuper == 0x3) {
            svm->vm->status.set(IsMonitoring, running);
            svm->vm->status.set(IsRunning, running);
            return;
        }
        if (isSuper == 0x1)
            svm->vm->status.set(IsMonitoring, running);
        else if (isSuper == 0x2)
            svm->vm->status.set(IsRunning, running);
    }

    void EmuThread::vmSupervisor(std::shared_ptr<SharedVm> svm) {
        pthread_setname_np(pthread_self(), "Vm.Monitor");
        std::reference_wrapper<std::atomic_bool> isAlive{svm->vm->status.running};
        svm->setMonitor((svrRunning << 8) | svrMonitor3);

        for (; (svm->getMonitor() >> 8) != svrFinished; ) {
            // The supervision thread will be executed every 95 milliseconds
            std::this_thread::sleep_for(std::chrono::nanoseconds(95'000));
            if (svm->getMonitor() & svrMonitor3) {
                if (isAlive.get())
                    isAlive = svm->vm->status.monitor;
            } else if (svm->getMonitor() & svrMonitor1) {
                if (isAlive.get())
                    continue;
                svm->setMonitor((svrFinished << 8) & 0xff00);

                isAlive = svm->vm->status.running;
            }
            [[likely]] if (isAlive.get()) {
                if (svm->getMonitor() & svrMonitor3) {
                    std::scoped_lock<std::mutex> scope(mlMutex);
                    svm->setMonitor(0xff00 | svrMonitor2);
                    mlCond.notify_one();
                } else if (svm->getMonitor() & svrMonitor2) {
                    // We're monitoring the EmuThread behavior
                    std::this_thread::yield();

                    svm->setMonitor(0xff00 | svrMonitor1);
                }
            }
        }
    }
    void EmuThread::switchVmPower(bool is) {
        if (is) {
            updateValues(vmSharedPtr, is, 0x3);
        } else {
            updateValues(vmSharedPtr, is, 0x1);
            updateValues(vmSharedPtr, is, 0x2);
        }
    }
    EmuThread::EmuThread(EmuVm& vm) {
        vmSharedPtr = std::make_shared<SharedVm>(vm);
    }
    void EmuThread::haltVm() {
        switchVmPower(false);
        if (vmThread.joinable())
            vmThread.join();
    }
    void EmuThread::runVm() {
        if (vmThread.joinable())
            ;
        auto superThread{std::thread(vmSupervisor, vmSharedPtr)};
        vmThread.swap(superThread);

        switchVmPower(true);
        vmMain(vmSharedPtr);
        vmThread.join();
    }
}
