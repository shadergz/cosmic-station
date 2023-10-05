#pragma once
#include <console/virtual_devices.h>
#include <link/glb_memory.h>

namespace zenith::console {
    class EmuVM {
    public:
        EmuVM(std::shared_ptr<link::GlobalMemory>& memory,
              std::shared_ptr<console::VirtualDevices>& devices) :
                emuMem(memory), emuDevs(devices) {}
    private:
        std::shared_ptr<link::GlobalMemory> emuMem;
        std::shared_ptr<console::VirtualDevices> emuDevs;
    };
}
