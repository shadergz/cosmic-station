#pragma once

#include <span>

#include <types.h>
#include <eeiv/ee_engine.h>
#include <kernel/group.h>
namespace zenith::kernel {
    class BiosHLE {
    public:
        BiosHLE(const std::shared_ptr<eeiv::EEMipsCore>& core)
            : group(std::make_shared<KernelsGroup>()),
              mips(core) {}
        void resetBIOS();

        std::shared_ptr<KernelsGroup> group;
    private:
        static u32 prodAsmIntHandler(std::span<u32> block);
        std::shared_ptr<eeiv::EEMipsCore> mips;

        u32 intCodeASM[81];
    };
}
