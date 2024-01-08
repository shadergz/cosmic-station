#pragma once

#include <console/virt_devices.h>
#include <console/intc.h>

#include <vm/sched_logical.h>
#include <vm/emu_thread.h>
#include <mio/main_memory.h>
#include <hle/bios_patch.h>
#include <hle/syscall_gate.h>
#include <gpu/exhibition_engine.h>
namespace cosmic::vm {
    class EmuVm {
    public:
        EmuVm(JNIEnv* env,
              std::shared_ptr<console::VirtDevices>& devices,
              std::shared_ptr<gpu::ExhibitionEngine>& dsp);

        void resetVm();
        void startVm();
        void stopVm();
        void dealWithSyscalls();

        std::shared_ptr<hle::BiosPatcher> biosHigh;
        std::shared_ptr<engine::EeMipsCore> mips;
        std::shared_ptr<iop::IoMipsCore> iop;
        std::shared_ptr<ipu::IpuMpeg2> mpegDecoder;
        std::shared_ptr<gs::GifBridge> gsGif;

        std::shared_ptr<gpu::ExhibitionEngine> screenEngine;
        u8 frames;

        std::shared_ptr<Scheduler> scheduler;
        std::shared_ptr<console::Vu01Package> vu01;
        std::shared_ptr<mio::MemoryPipe> sharedPipe;
        std::atomic<bool> hasFrame{false};
    private:
        EmuThread emuThread;
        std::shared_ptr<console::IntCInfra> intc;

        std::unique_ptr<hle::SyscallDealer> dealer;
    };
}
