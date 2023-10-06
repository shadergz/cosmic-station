#pragma once
#include <console/virtual_devices.h>
#include <link/glb_memory.h>

namespace zenith::console {
    class EmuVM {
    public:
        EmuVM(const std::shared_ptr<link::GlobalMemory>& memory,
              const std::shared_ptr<console::VirtualDevices>& devices);

        void resetVM();

        std::shared_ptr<kernel::BiosHLE> biosHLE;
    private:
        std::shared_ptr<link::GlobalMemory> emuMem;
        std::shared_ptr<eeiv::EEMipsCore> mips;
        std::shared_ptr<iop::IOMipsCore> iop;

        u8 frames;
    };
}
