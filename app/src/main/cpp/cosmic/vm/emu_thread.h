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
            vm = Optional(svm);
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
        Optional<EmuVm> vm;

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
        static void runFrameLoop(Optional<EmuVm>& vm);
        static void stepMips(Optional<EmuVm>& vm, u32 mips, u32 iop, u32 bus);
        static void stepVus(Optional<EmuVm>& vm, u32 mips, u32 bus);
        static void stepGs(Optional<EmuVm>& vm, u32 bus);

        std::thread vmThread;
        std::shared_ptr<SharedVm> vmSharedPtr;
    };
}
