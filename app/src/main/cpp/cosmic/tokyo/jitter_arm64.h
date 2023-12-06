#pragma once

#include <engine/ee_info.h>
namespace cosmic::engine::tokyo {
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

