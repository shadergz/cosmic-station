#pragma once

#include <thread>
#include <common/types.h>
namespace cosmic::vm {
    class EmuVm;

    struct SharedVm {
        SharedVm(EmuVm& svm) {
            vm = std::ref(svm);
            monitorStatus.store(0);
        }
        auto getMonitor(u16 mask = 0xffff) {
            return monitorStatus.load() & mask;
        }
        auto setMonitor(u16 value) {
            monitorStatus.store(value);
        }
        Ref<EmuVm> vm;
        std::atomic<u16> monitorStatus;
    };

    constexpr u8 svrFinished{0x85};
    constexpr u8 svrRunning{0x80};
    constexpr u8 svrMonitor1{0x10}; // Needs a check
    constexpr u8 svrMonitor2{0x20}; // Run an update
    constexpr u8 svrMonitor3{0x30}; // Initial state

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
