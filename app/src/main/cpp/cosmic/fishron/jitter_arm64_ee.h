#pragma once

#include <ee/ee_info.h>
#include <fishron/emitter_common.h>
namespace cosmic::fishron {
    class EeArm64Jitter : public ee::EeExecutor {
    public:
        EeArm64Jitter(Wrapper<ee::EeMipsCore> intCpu) :
            EeExecutor(intCpu) {}
        u32 executeCode() override;
        u32 fetchPcInst(u32 address) override;
        void performInvalidation(u32 address) override;

        std::unique_ptr<Emitter> emitter;
    };
}

