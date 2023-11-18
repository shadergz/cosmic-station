#pragma once

#include <span>
#include <array>

#include <common/types.h>
#include <hle/group_mgr.h>
#include <eeiv/ee_engine.h>

namespace cosmic::hle {
    class BiosPatcher {
    public:
        BiosPatcher(JNIEnv* env, std::shared_ptr<eeiv::EEMipsCore>& core)
            : group(std::make_shared<HLEBiosGroup>(env)),
              mips(core) {}
        void resetBIOS();
        void emit(u32 address);

        std::shared_ptr<HLEBiosGroup> group;
    private:
        u32 prodAsmIntHandler();
        void andIntCStatToT2(u32& range);
        void regsFromKernel0(u32& range, bool save);
        void intCAndJump(u32& range);

        std::shared_ptr<eeiv::EEMipsCore> mips;

        std::array<u32, 81> intCodeASM;
    };
}
