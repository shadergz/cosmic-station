#pragma once

#include <iop/iop_intc.h>
namespace cosmic::console {
    enum INTControllers {
        IopInt
    };
    class EmuVM;

    class INTCInfra {
    public:
        INTCInfra(EmuVM& vm);
        void checkInt(INTControllers tni);

        void resetPIC();

        iop::IopINTC iopInt;
    };
}
