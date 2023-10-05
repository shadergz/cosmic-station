#include <console/emu_vm.h>

namespace zenith::console {
    EmuVM::EmuVM(std::shared_ptr<link::GlobalMemory> &memory,
        std::shared_ptr<console::VirtualDevices> &devices)
            : emuMem(memory),
              mips(devices->mipsEER5900),
              iop(devices->mipsIOP) {

        biosHLE = std::make_unique<kernel::BiosHLE>(mips);
        frames = 30;
    }

    void EmuVM::resetVM() {
        mips->resetCore();
        mips->cop0.resetCOP();

        iop->resetIOP();
        biosHLE->resetBIOS();
    }
}
