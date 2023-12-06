#pragma once

#include <console/virt_devices.h>
#include <mio/main_memory.h>
#include <hle/bios_patch.h>
#include <hle/syscall_gate.h>
#include <gpu/exhibition_engine.h>

#include <console/vm/emu_thread.h>
#include <console/sched_logical.h>
#include <console/intc.h>
namespace cosmic::console::vm {
    class EmuVM {
    public:
        EmuVM(JNIEnv* env,
            std::shared_ptr<VirtDevices>& devices,
            std::shared_ptr<gpu::ExhibitionEngine>& dsp);

        void resetVM();
        void startVM();
        void stopVM();
        void dealWithSyscalls();

        std::shared_ptr<hle::BiosPatcher> biosHLE;

        std::shared_ptr<mio::DMAController> memCtrl;
        std::shared_ptr<eeiv::EeMipsCore> mips;
        std::shared_ptr<iop::IoMipsCore> iop;

        std::shared_ptr<gpu::ExhibitionEngine> screenEngine;
        u8 frames;

        std::shared_ptr<Scheduler> scheduler;
        std::shared_ptr<VU01Pack> vu01;
        std::atomic<bool> hasFrame{false};
    private:
        EmuThread emuThread;
        std::shared_ptr<INTCInfra> intc;

        hle::SyscallDealer dealer;
    };
}
