#pragma once

#include <span>
#include <impltypes.h>

namespace zenith::kernel {
    class BiosHLE {
    public:
        static u32 prodAsmIntHandler(std::span<u32>& block);
    };
}
