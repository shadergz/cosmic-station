#include <mio/blocks.h>

namespace cosmic::mio {
    u8* GlobalMemory::makeRealAddress(u32 address, RealAddressFrom mkFrom) {
        u32 realAddress;
        u8* hostPointer{};
        [[likely]] if (mkFrom == MainMemory) {
            realAddress = rdRam.resolve(address);
            hostPointer = rdRam.access(realAddress);
        } else if (mkFrom == BiosMemory) {
            realAddress = bios.resolve(address);
            hostPointer = bios.access(realAddress);
        } else if (mkFrom == IopMemory) {
            realAddress = iop.resolve(address);
            hostPointer = iop.access(realAddress);
        }
        return hostPointer;
    }

    u8* GlobalMemory::iopUnaligned(u32 address) {
        // IOP can only access its own RAM or the BIOS physically
        u8* blockPtr{};
        if (address < 0x00200000)
            blockPtr = iop.access(address);
        else if (address >= 0x1fc00000 && address < 0x20000000) {
            blockPtr = bios.access(address & 0x3fffff);
        }
        return blockPtr;
    }
}