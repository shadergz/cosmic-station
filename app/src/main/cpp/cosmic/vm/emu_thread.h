#pragma once

#include <thread>
#include <common/types.h>
namespace cosmic::vm {
    class EmuVm;
    enum MonitorMode {
        SvrNone = 0,
        SvrNeedsCheck,
        SvrRunAnUpdate,
        SvrNewState
    };
    struct SharedVm {
        SharedVm(EmuVm& svm) {
            vm = std::ref(svm);
            running.store(false);
            monitor.store(SvrNone);
        }
        auto setMode(MonitorMode mood) {
            auto before{monitor.load()};
            monitor.store(mood);
            return before;
        }
        auto getMode() const {
            return monitor.load();
        }
        auto setRunning(bool is) -> bool {
            auto was{running.load()};
            running.store(is);
            return was;
        }
        auto isRunning() const {
            return running.load();
        }
        Ref<EmuVm> vm;

        std::atomic<bool> running;
        std::atomic<MonitorMode> monitor;
    };
    class EmuThread {
    public:
        EmuThread(EmuVm& vm);
        void runVm();
        void haltVm();
        void switchVmPower(bool is);
    private:
        void updateValues(std::shared_ptr<SharedVm>& svm, bool running, u8 isSuper);
        static void vmMain(std::shared_ptr<SharedVm>& svm);
        static void vmSupervisor(std::shared_ptr<SharedVm> svm);
        static void runFrameLoop(Ref<EmuVm>& vm);
        static void stepMips(Ref<EmuVm>& vm, u32 mips, u32 iop, u32 bus);
        static void stepVus(Ref<EmuVm>& vm, u32 mips, u32 bus);

        std::thread vmThread;
        std::shared_ptr<SharedVm> vmSharedPtr;
    };
}
