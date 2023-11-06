#pragma once

#include <console/virt_devices.h>
#include <link/main_memory.h>
#include <gpu/hw_render.h>
#include <hle/bios_patch.h>
#include <gpu/exhibition_engine.h>

#include <console/emu_thread.h>
#include <console/sched_logical.h>
namespace zenith::console {
    class EmuVM {
    public:
        EmuVM(JNIEnv* env,
            std::shared_ptr<link::GlobalMemory>& memory,
            std::shared_ptr<VirtDevices>& devices,
            std::shared_ptr<gpu::ExhibitionEngine>& dsp);

        void resetVM();
        void startVM();

        std::shared_ptr<hle::BiosPatcher> biosHLE;

        std::shared_ptr<link::GlobalMemory> emuMem;
        std::shared_ptr<gpu::ExhibitionEngine> screenEngine;
        std::shared_ptr<eeiv::EEMipsCore> mips;
        std::shared_ptr<iop::IOMipsCore> iop;

        std::unique_ptr<gpu::RenderScene> render;
        u8 frames;

        std::shared_ptr<Scheduler> scheduler;
    private:
        EmuThread emuThread;
    };
}
