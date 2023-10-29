#include <console/emu_vm.h>

namespace zenith::console {
    EmuVM::EmuVM(JNIEnv* env,
        std::shared_ptr<link::GlobalMemory>& memory,
        std::shared_ptr<console::VirtualDevices>& devices)
            : emuMem(memory),
              mips(devices->mipsEER5900),
              iop(devices->mipsIOP) {

        biosHLE = std::make_shared<hle::BiosPatcher>(env, mips);
        render = std::make_unique<gpu::RenderScene>();

        frames = 30;
    }

    void EmuVM::startVM() {
        render->estUserRender();
    }

    void EmuVM::resetVM() {
        // Resetting all co-processors
        mips->cop0.resetCoP();
        mips->resetCore();

        iop->resetIOP();
        biosHLE->resetBIOS();
    }
}
