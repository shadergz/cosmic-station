#pragma once

#include <memory>

#include <console/virtual_devices.h>
#include <console/emu_vm.h>
#include <global.h>

namespace zenith {
    class CoreApplication {
    public:
        CoreApplication();
        std::shared_ptr<kernel::KernelsGroup> getKernelsGroup();
    private:
        std::shared_ptr<link::GlobalMemory> virBlocks;
        std::shared_ptr<console::VirtualDevices> simulated;
        std::unique_ptr<console::EmuVM> vm;
    };
    extern std::shared_ptr<CoreApplication> zenithApp;
}
