#pragma once

#include <eeiv/EE-Handler.h>
namespace zenith::eeiv::casper {
    class EEInterpreter : public EEExecutor {
    public:
        EEInterpreter(EEMipsCore& intCpu) : EEExecutor(intCpu) {}

        u32 execCode() override {
            return 0;
        }
    };

}
