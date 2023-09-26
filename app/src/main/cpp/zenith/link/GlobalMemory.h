#pragma once

#include <os/HostMemory.h>
namespace zenith::console {
    class GlobalMemory {
    public:
        static inline u32 resolveBios(u32 address) {
            return address &= 1024 * 1024 * 4 - 1;
        }
        static inline u32 resolveDRAM(u32 address) {
            return address &= 1024 * 1024 * 32 - 1;
        }

        inline u8* makeRealAddress(u32 address, bool isBios = false) {
            u32 realAddress{};
            [[likely]] if (!isBios)
                realAddress = resolveDRAM(address);
            else
                realAddress = resolveBios(address);

            return &RDRAMBlk[realAddress];
        }
    private:
        // Allocating 32 megabytes of RAM to the primary CPU
        // In a simulated hardware environment, we could simply create an array of bytes to serve
        // as our RAM without any issues
        os::MappedMemory<u8> RDRAMBlk{static_cast<uint64_t>(1024 * 1024 * 32)};
    };
}
