#pragma once

#include <thread>
namespace cosmic::console::vm {
    class EmuVM;
    struct EmuShared {
        std::atomic<bool> isRunning{false};
        std::atomic<bool> isMonitoring{false};
        std::atomic<u64> executionCount{};
        raw_reference<EmuVM> frame{};
    };

    class EmuThread {
    public:
        EmuThread(EmuVM& vm);
        void runVM();
        void haltVM();
        void switchVmPower(bool is);
    private:
        static void vmMain(std::shared_ptr<EmuShared> owner);
        static void vmSupervisor(std::shared_ptr<EmuShared> owner);
        static void runFrameLoop(std::shared_ptr<EmuShared> owner);

        std::thread vmt;
        static std::shared_ptr<EmuShared> shared;
    };
}
