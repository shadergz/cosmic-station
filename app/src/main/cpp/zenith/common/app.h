#pragma once

#include <memory>

#include <console/virt_devices.h>
#include <console/emu_vm.h>
#include <link/blocks.h>
#include <cpu/verify_features.h>
#include <hle/group_mgr.h>
#include <gpu/exhibition_engine.h>

namespace zenith {
    class CoreApplication {
    public:
        CoreApplication();
        std::shared_ptr<hle::HLEBiosGroup> getBiosMgr();
        const std::string& getDeviceName();

        std::string lastSetSync;
        std::unique_ptr<console::EmuVM> vm;
    private:
        std::shared_ptr<link::GlobalMemory> virtBlocks;
        std::shared_ptr<console::VirtDevices> simulated;
        std::shared_ptr<gpu::ExhibitionEngine> scene;

        cpu::HostFeatures riscFeatures{};

        i32 apiLevel{-1};
        std::string artDeviceName{};
    };
}
