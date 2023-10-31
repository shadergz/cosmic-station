#pragma once

#include <memory>

#include <console/virtual_devices.h>
#include <console/emu_vm.h>
#include <link/blocks.h>
#include <cpu/verify_features.h>
#include <hle/group_mgr.h>

namespace zenith {
    class CoreApplication {
    public:
        CoreApplication();
        std::shared_ptr<hle::HLEBiosGroup> getBiosMgr();
        const std::string& getDeviceName();

        std::string lastSetSync;
        std::unique_ptr<console::EmuVM> vm;
    private:
        std::shared_ptr<link::GlobalMemory> virBlocks;
        std::shared_ptr<console::VirtualDevices> simulated;

        cpu::HostFeatures riscFeatures{};

        i32 apiLevel{-1};
        std::string artDeviceName{};
    };
}
