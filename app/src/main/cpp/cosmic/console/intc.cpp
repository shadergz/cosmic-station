#include <console/intc.h>
#include <vm/emu_vm.h>
namespace cosmic::console {
    IntCInfra::IntCInfra(vm::EmuVm& vm) :
        iopInt(vm.iop),
        eeInt(vm.mips, vm.scheduler)
        {
    }
    void IntCInfra::resetPic() {
        // iopInt.stat = 0;
        // iopInt.mask = 0;
        // iopInt.ctrl = 0;
        eeInt.resetEeInterrupt();
        iopInt.resetInterrupt();
    }
    void IntCInfra::checkInt(IntControllers tni) {
        switch (tni) {
        case IopInt:
            iopInt.iopCheck(); break;
        case EeInt:
            break;
        }
    }
    void IntCInfra::trapIrq(IntControllers in, u8 id) {
        if (in == EeInt) {
            eeInt.raiseIrq(id);
        }
    }
}