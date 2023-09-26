#pragma once

#include <eeiv/EE-Handler.h>
namespace zenith::eeiv::tokyo3 {
    class EEArm64Jitter : public EEExecutor {
    public:
        EEArm64Jitter(EEMipsCore& intCpu) : EEExecutor(intCpu) {}

        u32 execCode() override {
            return 0;
        }
    };

}

