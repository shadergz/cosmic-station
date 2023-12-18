#pragma once

#include <thread>
#include <common/types.h>
namespace cosmic::vm {
    class EmuVM;
    struct EmuShared {
        std::atomic<bool> isRunning{false};
        std::atomic<bool> isMonitoring{false};
        std::atomic<u64> executionCount{};
        std::atomic<u16> check;
        raw_reference<EmuVM> frame{};
    };
    constexpr u8 svrFinished{0x85};
    constexpr u8 svrRunning{0x80};
    constexpr u8 svrMonitor1{0x10}; // Needs a check
    constexpr u8 svrMonitor2{0x20}; // Run an update
    constexpr u8 svrMonitor3{0x30}; // Initial state

    class EmuThread {
    public:
        EmuThread(EmuVM& vm);
        void runVM();
        void haltVM();
        void switchVmPower(bool is);
    private:
        void updateValues(bool running, u8 isSuper);
        static void vmMain(std::shared_ptr<EmuShared> owner);
        static void vmSupervisor(std::shared_ptr<EmuShared> owner);
        static void runFrameLoop(std::shared_ptr<EmuShared> owner);
        static void stepMips(u32 mips, u32 iop, u32 bus, raw_reference<EmuVM> vm);
        static void stepVus(u32 mips, u32 bus, raw_reference<EmuVM> vm);

        std::thread vmt;
        std::shared_ptr<EmuShared> shared;
    };
}
