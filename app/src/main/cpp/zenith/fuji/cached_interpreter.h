#pragma once

#include <eeiv/ee_handler.h>
namespace zenith::eeiv::fuji {
    class EEInterpreter : public EEExecutor {
    public:
        EEInterpreter(EEMipsCore& intCpu) : EEExecutor(intCpu) {}
        u32 execCode() override;
    };
}
