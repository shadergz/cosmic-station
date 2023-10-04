#pragma once

#include <span>
#include <impltypes.h>

#include <eeiv/ee_engine.h>
namespace zenith::kernel {
    class BiosHLE {
    public:
        BiosHLE(const std::shared_ptr<eeiv::EEMipsCore>& core) :
            mips(core) {}
        void resetBIOS();
    private:
        static u32 prodAsmIntHandler(std::span<u32> block);
        std::shared_ptr<eeiv::EEMipsCore> mips;

        u32 intCodeASM[64];
    };
}
