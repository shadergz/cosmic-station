// SPDX-short-identifier: MIT, Version N/A
// This file is protected by the MIT license (please refer to LICENSE.md before making any changes, copying, or redistributing this software)
#include <console/emu_vm.h>
#include <common/global.h>

namespace zenith::console {
    EmuVM::EmuVM(JNIEnv* env,
        std::shared_ptr<link::GlobalMemory>& memory,
        std::shared_ptr<VirtDevices>& devices,
        std::shared_ptr<gpu::ExhibitionEngine>& dsp)
            : emuMem(memory),
              screenEngine(dsp),
              mips(devices->mipsEER5900),
              iop(devices->mipsIOP),
              emuThread(*this) {

        biosHLE = std::make_shared<hle::BiosPatcher>(env, mips);
        scheduler = std::make_shared<Scheduler>();
        render = std::make_unique<gpu::RenderScene>();

        mips->timer.wakeUp = scheduler;
        frames = 30;
    }

    void EmuVM::startVM() {
        userLog->info("Starting VM from an improper context; this should be fixed later");
        render->pickUserRender();

        std::span<u8> eeKernelRegion{emuMem->makeRealAddress(0, true), emuMem->biosSize()};
        try {
            biosHLE->group->readBios(eeKernelRegion);
            emuThread.runVM();
        } catch (const NonAbort& except) {
            return;
        }
    }

    void EmuVM::resetVM() {
        scheduler->resetCycles();

        // Resetting all co-processors
        mips->cop0.resetCoP();
        mips->fuCop1.resetFlu();
        mips->timer.resetTimers();

        mips->resetCore();
        iop->resetIOP();

        biosHLE->resetBIOS();
    }
}
