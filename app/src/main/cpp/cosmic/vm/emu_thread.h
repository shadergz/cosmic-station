#pragma once

#include <thread>
#include <common/types.h>
namespace cosmic::vm {
    class EmuVm;
    struct EmuShared {
        std::atomic<bool> isRunning{false};
        std::atomic<bool> isMonitoring{false};
        std::atomic<u64> executionCount{};
        std::atomic<u16> check;
        RawReference<EmuVm> frame{};
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
        void updateValues(bool running, u8 isSuper);
        static void vmMain(std::shared_ptr<EmuShared> owner);
        static void vmSupervisor(std::shared_ptr<EmuShared> owner);
        static void runFrameLoop(std::shared_ptr<EmuShared> owner);
        static void stepMips(u32 mips, u32 iop, u32 bus, RawReference<EmuVm> vm);
        static void stepVus(u32 mips, u32 bus, RawReference<EmuVm> vm);

        std::thread vmt;
        std::shared_ptr<EmuShared> shared;
    };
}
