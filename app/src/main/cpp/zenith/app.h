#pragma once

#include <memory>

#include <console/virtual_devices.h>
#include <console/emu_vm.h>
#include <link/blocks.h>
#include <cpu/verify_features.h>
#include <global.h>

namespace zenith {
    class CoreApplication {
    public:
        CoreApplication();
        std::shared_ptr<kernel::KernelsGroup> getKernelsGroup();

        const std::string& getDeviceName();
    private:
        std::shared_ptr<link::GlobalMemory> virBlocks;
        std::shared_ptr<console::VirtualDevices> simulated;
        std::unique_ptr<console::EmuVM> vm;

        cpu::HostFeatures riscFeatures{};

        i32 apiLevel{-1};
        std::string artDeviceName{};
    };
    extern std::shared_ptr<CoreApplication> zenithApp;
}
