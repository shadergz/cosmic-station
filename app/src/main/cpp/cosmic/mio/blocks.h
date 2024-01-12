#pragma once

#include <os/mapped.h>
namespace cosmic::mio {
    enum RealAddressFrom {
        NormalAddressing,
        MainMemory,
        IopMemory,
        Spu2Ram,
        BiosMemory,
    };

    class GlobalMemory {
    public:
        u8* makeRealAddress(u32 address, RealAddressFrom mkFrom = MainMemory);
        auto biosSize() {
            return dynEprom.getBlockSize();
        }
        u8* iopUnaligned(u32 address);
        u8* spu2Unaligned(u32 address);
    private:
        u8* access(u32 address, RealAddressFrom from);

        os::MappedMemory<u8> iopBlock{static_cast<uint64_t>(1024 * 1024 * 2)};
        os::MappedMemory<u8> soundBlock{static_cast<uint64_t>(1024 * 1024 * 2)};
        // Allocating 32 megabytes of RAM to the primary CPU
        os::MappedMemory<u8> rdRamBlock{static_cast<uint64_t>(1024 * 1024 * 32)};

        os::MappedMemory<u8> dynEprom{static_cast<uint64_t>(1024 * 1024 * 4)};
    };
}
