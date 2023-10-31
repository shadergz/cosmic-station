#pragma once

#include <link/main_memory.h>
#include <link/bios_memory.h>

namespace zenith::link {
    class GlobalMemory {
    public:
        inline u8* makeRealAddress(u32 address, bool isBios = false) {
            u32 realAddress;
            [[likely]] if (!isBios)
                realAddress = rdRam.resolve(address);
            else
                realAddress = bios.resolve(address);

            return !isBios ? rdRam.access(realAddress) : bios.access(realAddress);
        }
        auto biosSize() {
            return bios.dynEPROM.getBlockSize();
        }
    private:
        LogicalRAMBlock rdRam{};
        BIOSBlock bios{};
    };
}
