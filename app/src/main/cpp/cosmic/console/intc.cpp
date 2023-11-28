#include <console/intc.h>
#include <console/vm/emu_vm.h>
namespace cosmic::console {
    INTCInfra::INTCInfra(vm::EmuVM& vm)
        : iopInt(vm.iop) {
    }
    void INTCInfra::resetPIC() {
        iopInt.stat = 0;
        iopInt.mask = 0;
        iopInt.ctrl = 0;
    }
    void INTCInfra::checkInt(INTControllers tni) {
        switch (tni) {
        case IopInt:
            iopInt.iopCheck();
            break;
        }
    }
}