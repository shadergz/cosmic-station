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
        u8* mapVirtAddress(u32 address, RealAddressFrom mkFrom = MainMemory);
        u64 biosSize() {
            return static_cast<u64>(&ramBlock[0x3fffff] - &ramBlock[0]) + 1;
        }
        u8* iopUnaligned(u32 address);
        u8* spu2Unaligned(u32 address);
    private:
        u8* access(u32 address, RealAddressFrom from);

        os::MappedMemory<u8> iopBlock{static_cast<uint64_t>(1024 * 1024 * 2)};
        os::MappedMemory<u8> soundBlock{static_cast<uint64_t>(1024 * 1024 * 2)};
        // Allocating 32 megabytes of RAM to the primary CPU
        os::MappedMemory<u8> ramBlock{static_cast<uint64_t>(1024 * 1024 * 32)};
    };
}
