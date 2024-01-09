#pragma once

#include <iop/iop_intc.h>
#include <engine/ee_intc.h>
namespace cosmic::vm {
    class EmuVm;
}
namespace cosmic::console {
    enum IntControllers {
        EeInt,
        IopInt,
    };

    class IntCInfra {
    public:
        IntCInfra(vm::EmuVm& vm);
        void checkInt(IntControllers tni);
        void trapIrq(IntControllers in, u8 id);
        void resetPic();

        iop::IopIntC iopInt;
        engine::EeIntC eeInt;
    };
}
