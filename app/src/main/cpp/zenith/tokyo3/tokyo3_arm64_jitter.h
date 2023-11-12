#pragma once

#include <eeiv/ee_fuji.h>
namespace zenith::eeiv::tokyo3 {
    class EEArm64Jitter : public EEExecutor {
    public:
        EEArm64Jitter(EEMipsCore& intCpu) : EEExecutor(intCpu) {}
        u32 executeCode() override {
            return 0;
        }
    };

}

