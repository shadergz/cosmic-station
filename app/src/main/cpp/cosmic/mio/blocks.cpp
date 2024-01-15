#include <mio/blocks.h>

namespace cosmic::mio {
    u8* GlobalMemory::mapVirtAddress(u32 address, RealAddressFrom mkFrom) {
        u32 realAddress;
        u8* hostPointer{};
        [[likely]] if (mkFrom == MainMemory) {
            realAddress = address & (1024 * 1024 * 32 - 1);
            hostPointer = access(realAddress, MainMemory);
        } else if (mkFrom == BiosMemory) {
            realAddress = address & (1024 * 1024 * 4 - 1);
            hostPointer = access(realAddress, MainMemory);
        } else if (
                mkFrom == IopMemory ||
                mkFrom == Spu2Ram) {
            realAddress = address & (1024 * 1024 * 2 - 1);
            hostPointer = access(realAddress, mkFrom);
        }
        return hostPointer;
    }

    u8* GlobalMemory::iopUnaligned(u32 address) {
        // IOP can only access its own RAM or the BIOS physically
        u8* blockPtr{};
        if (address < 0x00200000)
            blockPtr = access(address, IopMemory);
        else if (address >= 0x1fc00000 && address < 0x20000000) {
            blockPtr = access(address & 0x3fffff, BiosMemory);
        }
        return blockPtr;
    }
    u8* GlobalMemory::spu2Unaligned(u32 address) {
        return access(address, Spu2Ram);
    }

    u8* GlobalMemory::access(u32 address, RealAddressFrom from) {
        switch (from) {
        case IopMemory:
            return &iopBlock[address];
        case BiosMemory:
        case MainMemory:
            return &ramBlock[address];
        case Spu2Ram:
            return &soundBlock[address];
        }
    }
}