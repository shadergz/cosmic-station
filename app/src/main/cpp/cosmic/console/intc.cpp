#include <console/intc.h>
#include <vm/emu_vm.h>
namespace cosmic::console {
    IntCInfra::IntCInfra(vm::EmuVm& vm)
        : iopInt(vm.iop) {
    }
    void IntCInfra::resetPic() {
        iopInt.stat = 0;
        iopInt.mask = 0;
        iopInt.ctrl = 0;
    }
    void IntCInfra::checkInt(IntControllers tni) {
        switch (tni) {
        case IopInt:
            iopInt.iopCheck(); break;
        }
    }
}