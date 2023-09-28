#pragma once

#include <eeiv/ee_handler.h>
namespace zenith::eeiv::casper {
    class EEInterpreter : public EEExecutor {
    public:
        EEInterpreter(EEMipsCore& intCpu) : EEExecutor(intCpu) {}

        u32 execCode() override {
            return 0;
        }
    };

}
