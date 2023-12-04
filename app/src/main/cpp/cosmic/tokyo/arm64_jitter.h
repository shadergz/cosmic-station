#pragma once

#include <eeiv/ee_fuji.h>
namespace cosmic::eeiv::tokyo {
    class EeArm64Jitter : public EeExecutor {
    public:
        EeArm64Jitter(EeMipsCore& intCpu) : EeExecutor(intCpu) {}
        u32 executeCode() override {
            return 0;
        }
        u32 fetchPcInst() override {
            return 0;
        }
    };

}

