#pragma once

#include <engine/ee_info.h>
namespace cosmic::tokyo {
    class EeArm64Jitter : public engine::EeExecutor {
    public:
        EeArm64Jitter(engine::EeMipsCore& intCpu) : EeExecutor(intCpu) {}
        u32 executeCode() override {
            return 0;
        }
        u32 fetchPcInst() override {
            return 0;
        }
    };
    struct StackOperation {
        std::array<char, 32> op;
        u16 operate;
        union {
            i16 spo;
            u16 offset;
        };
        u32 arm;
    };
}

