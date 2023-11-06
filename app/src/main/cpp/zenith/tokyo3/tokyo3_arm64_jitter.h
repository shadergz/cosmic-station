#pragma once

#include <eeiv/ee_handler.h>
namespace zenith::eeiv::tokyo3 {
    class EEArm64Jitter : public EEExecutor {
    public:
        EEArm64Jitter(EEMipsCore& intCpu) : EEExecutor(intCpu) {}
        u32 execCode() override {
            return 0;
        }
    };

}

