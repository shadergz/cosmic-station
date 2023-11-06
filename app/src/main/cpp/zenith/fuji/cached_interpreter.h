#pragma once

#include <eeiv/ee_handler.h>
namespace zenith::fuji {
    class EEInterpreter : public eeiv::EEExecutor {
    public:
        EEInterpreter(eeiv::EEMipsCore& intCpu) : EEExecutor(intCpu) {}
        u32 execCode() override;
    };
}
