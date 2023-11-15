#pragma once

#include <mio/main_memory.h>
#include <mio/bios_memory.h>
#include <mio/iop_memory.h>
namespace zenith::mio {
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
            return bios.dynEPROM.getBlockSize();
        }
        u8* iopUnalignedRead(u32 address);
    private:
        LogicalRAMBlock rdRam{};
        BIOSBlock bios{};
        IOPMemory iop{};
    };
}
