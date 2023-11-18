#include <hle/syscall_gate.h>
#include <console/emu_vm.h>

namespace cosmic::hle {
    void SyscallDealer::hleResetEE(raw_reference<console::EmuVM> vm) {
        i32 resetParam{*vm->mips->gprAt<i32>(Param0)};
        if (resetParam == 0) {}
        else if (resetParam == 1) {}
    }
    void SyscallDealer::doSyscall(SyscallOrigin origin, i16 sys) {
        auto vm{redBox->openVm()};
        if (origin == SysEmotionEngine) {
            switch (sys) {
            case 0x01: // void ResetEE(i32 resetFlag);
                hleResetEE(vm); break;
            }
        }
        redBox->leaveVm(vm);
    }
}
