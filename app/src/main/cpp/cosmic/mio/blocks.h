#pragma once

#include <mio/main_memory.h>
#include <mio/bios_memory.h>
#include <mio/iop_memory.h>
namespace cosmic::mio {
    enum RealAddressFrom {
        NormalAddressing,
        MainMemory,
        BiosMemory,
        IopMemory,
    };
    class GlobalMemory {
    public:
        u8* makeRealAddress(u32 address, RealAddressFrom mkFrom = MainMemory);
        auto biosSize() {
            return bios.dynEprom.getBlockSize();
        }
        u8* iopUnaligned(u32 address);
    private:
        LogicalRamBlock rdRam{};
        BiosBlock bios{};
        IopMemory iop{};
    };
}
