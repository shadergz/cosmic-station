#pragma once

#include <console/virt_devices.h>
#include <console/intc.h>

#include <vm/sched_logical.h>
#include <vm/emu_thread.h>
#include <vm/watch.h>
#include <hle/bios_patch.h>
#include <hle/syscall_gate.h>
#include <gpu/exhibition_engine.h>
namespace cosmic::vm {
    class EmuVm {
    public:
        EmuVm(
            std::shared_ptr<console::VirtDevices>& devices,
            std::shared_ptr<gpu::ExhibitionEngine>& dsp);

        void resetVm();
        void startVm();
        void stopVm();
        void dealWithSyscalls();

        std::shared_ptr<hle::BiosPatcher> biosHigh;
        std::shared_ptr<engine::EeMipsCore> mips;
        std::shared_ptr<iop::IoMipsCore> iop;
        std::shared_ptr<iop::IopDma> ioDma;
        std::shared_ptr<spu::Spu2> sound;

        std::shared_ptr<ipu::IpuMpeg2> mpegDecoder;
        std::shared_ptr<gs::GsEngine> gsCore;
        std::shared_ptr<gs::GifBridge> gsGif;

        std::shared_ptr<gpu::ExhibitionEngine> screenEngine;

        std::shared_ptr<Scheduler> scheduler;
        std::shared_ptr<console::Vu01Package> vu01;
        std::shared_ptr<mio::MemoryPipe> sharedPipe;

        WatchStatus status{};
    private:
        EmuThread emuThread;
        std::shared_ptr<console::IntCInfra> intc;

        std::unique_ptr<hle::SyscallDealer> dealer;
    };
}
