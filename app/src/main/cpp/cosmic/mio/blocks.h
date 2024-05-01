#pragma once

#include <os/mapped.h>
namespace cosmic::mio {
    enum RealAddressFrom {
        MainMemory,
        BiosMemory,
        IopMemory,
        Spu2Ram,
    };

    class GlobalMemory {
    public:
        GlobalMemory();

        u8* mapVirtAddress(u32 address, RealAddressFrom mkFrom = MainMemory);
        u64 biosSize() {
            return static_cast<u64>(&ramBlock[0x3fffff] - &ramBlock[0]) + 1;
        }
        u8* iopUnaligned(u32 address);
        u8* spu2Unaligned(u32 address);

        void iopSoftClean() {
            memset(*iopBlock, 0, iopBlock.getBlockSize());
        }
    private:
        u8* access(u32 address, RealAddressFrom from);
        // Our unified memory strategy pointer
        os::MappedMemory<u8> umm;
        // Below, all unified memory regions related to console activities
        os::MappedMemory<u8> iopBlock;
        os::MappedMemory<u8> sndBlock;
        os::MappedMemory<u8> ramBlock;
    };
}
