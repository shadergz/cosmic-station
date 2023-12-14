#pragma once

#include <engine/ee_info.h>
#include <mahiro/emitter_common.h>
namespace cosmic::mahiro {
    class EeArm64Jitter : public engine::EeExecutor {
    public:
        EeArm64Jitter(engine::EeMipsCore& intCpu) :
            EeExecutor(intCpu) {}
        u32 executeCode() override;
        u32 fetchPcInst(u32 address) override;
        void performInvalidation(u32 address) override;

        std::unique_ptr<Emitter> emitter;
    };
}

