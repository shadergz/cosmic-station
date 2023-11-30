#pragma once

#include <eeiv/ee_fuji.h>
namespace cosmic::eeiv::tokyo3 {
    class EEArm64Jitter : public EeExecutor {
    public:
        EEArm64Jitter(EeMipsCore& intCpu) : EeExecutor(intCpu) {}
        u32 executeCode() override {
            return 0;
        }
        u32 fetchPcInst() override {
            return 0;
        }
    };

}

