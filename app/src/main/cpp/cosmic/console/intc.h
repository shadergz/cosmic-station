#pragma once

#include <iop/iop_intc.h>
namespace cosmic::vm {
    class EmuVM;
}
namespace cosmic::console {
    enum IntControllers {
        IopInt
    };

    class IntCInfra {
    public:
        IntCInfra(vm::EmuVM& vm);
        void checkInt(IntControllers tni);
        void resetPic();

        iop::IopIntC iopInt;
    };
}
