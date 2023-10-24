#pragma once

#include <span>
#include <array>

#include <types.h>
#include <eeiv/ee_engine.h>
#include <kernel/group_mgr.h>
namespace zenith::kernel {
    class BiosHLE {
    public:
        BiosHLE(JNIEnv* env, std::shared_ptr<eeiv::EEMipsCore>& core)
            : group(std::make_shared<KernelsGroup>(env)),
              mips(core) {}
        void resetBIOS();
        void emit(u32 address);

        std::shared_ptr<KernelsGroup> group;
    private:
        u32 prodAsmIntHandler();
        void andIntCStatToT2(u32& range);
        void regsFromKernel0(u32& range, bool save);
        void intCAndJump(u32& range);

        std::shared_ptr<eeiv::EEMipsCore> mips;

        std::array<u32, 81> intCodeASM;
    };
}
