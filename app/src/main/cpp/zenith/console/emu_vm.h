#pragma once

#include <console/virtual_devices.h>
#include <link/main_memory.h>
#include <gpu/hardware_render.h>
#include <hle/bios_patch.h>
namespace zenith::console {
    class EmuVM {
    public:
        EmuVM(JNIEnv* env,
              std::shared_ptr<link::GlobalMemory>& memory,
              std::shared_ptr<console::VirtualDevices>& devices);

        void resetVM();
        void startVM();

        std::shared_ptr<hle::BiosPatcher> biosHLE;
    private:
        std::shared_ptr<link::GlobalMemory> emuMem;
        std::shared_ptr<eeiv::EEMipsCore> mips;
        std::shared_ptr<iop::IOMipsCore> iop;

        std::unique_ptr<gpu::RenderScene> render;
        u8 frames;
    };
}
