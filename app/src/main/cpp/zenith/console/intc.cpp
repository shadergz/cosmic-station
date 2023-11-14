#include <console/intc.h>
#include <console/emu_vm.h>
namespace zenith::console {
    INTCInfra::INTCInfra(EmuVM& vm)
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