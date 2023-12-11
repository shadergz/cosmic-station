#pragma once

#include <engine/ee_info.h>
#include <mahiro/emitter_common.h>
namespace cosmic::mahiro {
    class EeArm64Jitter : public engine::EeExecutor {
    public:
        EeArm64Jitter(engine::EeMipsCore& intCpu) : EeExecutor(intCpu) {}
        u32 executeCode() override {
            return 0;
        }
        u32 fetchPcInst() override {
            return 0;
        }
        std::unique_ptr<Emitter> emitter;
    };
}

