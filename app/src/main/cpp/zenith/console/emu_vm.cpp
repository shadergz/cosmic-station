#include <console/emu_vm.h>

namespace zenith::console {
    EmuVM::EmuVM(JNIEnv* env,
        std::shared_ptr<link::GlobalMemory>& memory,
        std::shared_ptr<console::VirtualDevices>& devices,
        std::shared_ptr<gpu::ExhibitionEngine>& display)
            : emuMem(memory),
              exhEngine(display),
              mips(devices->mipsEER5900),
              iop(devices->mipsIOP) {

        biosHLE = std::make_shared<hle::BiosPatcher>(env, mips);
        render = std::make_unique<gpu::RenderScene>();

        frames = 30;
    }

    void EmuVM::startVM() {
        render->pickUserRender();

        std::span<u8> eeKernelRegion{emuMem->makeRealAddress(0, true), emuMem->biosSize()};
        try {
            biosHLE->group->readBios(eeKernelRegion);
        } catch (const NonAbort& except) {
            return;
        }
    }

    void EmuVM::resetVM() {
        // Resetting all co-processors
        mips->cop0.resetCoP();
        mips->resetCore();

        iop->resetIOP();
        biosHLE->resetBIOS();
    }
}
