#pragma once

#include <iop/iop_intc.h>
namespace cosmic::console::vm {
    class EmuVM;
}
namespace cosmic::console {
    enum INTControllers {
        IopInt
    };

    class INTCInfra {
    public:
        INTCInfra(vm::EmuVM& vm);
        void checkInt(INTControllers tni);
        void resetPIC();

        iop::IopINTC iopInt;
    };
}
