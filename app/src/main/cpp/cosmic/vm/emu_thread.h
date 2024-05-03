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
    struct ServerHyper {
        bool isRunning{false};
        MonitorMode mode{SvrNone};
    };

    struct SharedVm {
        SharedVm(EmuVm& svm) {
            vm = std::ref(svm);
            monitorStatus.store({
                .isRunning = false,
                .mode = SvrNewState
            });
        }
        auto getMonitor(MonitorMode testMode) {
            return monitorStatus.load().mode == testMode;
        }
        auto setMonitor(MonitorMode checkMode) {
            ServerHyper svr{monitorStatus.load()};
            auto lastMode{svr.mode};
            svr.mode = checkMode;
            monitorStatus.store(svr);
            return lastMode;
        }
        auto isRunning(bool is) {
            ServerHyper svr{monitorStatus.load()};
            auto last{svr.isRunning};
            svr.isRunning = is;
            return last;
        }
        auto isRunning() const {
            return monitorStatus.load().isRunning;
        }
        auto getMode() const {
            return monitorStatus.load().mode;
        }
        Ref<EmuVm> vm;

        std::atomic<ServerHyper> monitorStatus;
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
