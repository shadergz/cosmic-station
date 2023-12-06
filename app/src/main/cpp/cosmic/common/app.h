#pragma once

#include <memory>

#include <console/virt_devices.h>
#include <console/vm/emu_vm.h>
#include <cpu/verify_features.h>
#include <hle/group_mgr.h>
#include <gpu/exhibition_engine.h>

namespace cosmic {
    class CoreApplication {
    public:
        CoreApplication();
        std::shared_ptr<hle::HleBiosGroup> getBiosMgr();
        const std::string& getDeviceName();

        std::string lastSetSync;
        std::unique_ptr<console::vm::EmuVM> vm;
        std::shared_ptr<gpu::ExhibitionEngine> scene;
    private:
        std::shared_ptr<console::VirtDevices> simulated;

        cpu::HostFeatures riscFeatures{};

        i32 apiLevel{-1};
        std::string artDeviceName{};
    };
}
