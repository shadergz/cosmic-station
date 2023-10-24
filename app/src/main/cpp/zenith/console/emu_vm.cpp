#include <console/emu_vm.h>

namespace zenith::console {
    EmuVM::EmuVM(JNIEnv* env,
        std::shared_ptr<link::GlobalMemory>& memory,
        std::shared_ptr<console::VirtualDevices>& devices)
        : emuMem(memory),
          mips(devices->mipsEER5900),
          iop(devices->mipsIOP) {

        biosHLE = std::make_shared<kernel::BiosHLE>(env, mips);
        frames = 30;
    }

    void EmuVM::resetVM() {
        // Resetting all co-processors
        mips->cop0.resetCoP();
        mips->resetCore();

        iop->resetIOP();
        biosHLE->resetBIOS();
    }
}
