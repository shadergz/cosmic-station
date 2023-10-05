#pragma once

#include <memory>

#include <console/virtual_devices.h>
#include <console/emu_vm.h>
#include <global.h>

namespace zenith {
    class CoreApplication {
    public:
        CoreApplication();
    private:
        std::shared_ptr<link::GlobalMemory> virBlocks;
        std::shared_ptr<console::VirtualDevices> simulated;

        std::unique_ptr<console::EmuVM> driver;
    };
    extern std::unique_ptr<CoreApplication> zenithApp;
}
