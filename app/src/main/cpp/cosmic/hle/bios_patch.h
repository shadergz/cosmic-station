#pragma once

#include <span>
#include <array>

#include <common/types.h>
#include <hle/group_mgr.h>
#include <engine/ee_core.h>
namespace cosmic::hle {
    class BiosPatcher {
    public:
        BiosPatcher(std::shared_ptr<engine::EeMipsCore>& core) :
            group(std::make_shared<HleBiosGroup>()),
            mips(core) {
        }
        void resetBios();
        void emit(u32 address);

        std::shared_ptr<HleBiosGroup> group;
    private:
        u32 prodAsmIntHandler();
        void andIntCStatToT2(u32& range);
        void regsFromKernel0(u32& range, bool save);
        void intCAndJump(u32& range);

        std::shared_ptr<engine::EeMipsCore> mips;
        std::array<u32, 81> intCodeAsm;
    };
}
