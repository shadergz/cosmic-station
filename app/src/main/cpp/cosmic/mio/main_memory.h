#pragma once

#include <os/mapped.h>
namespace cosmic::mio {
    class LogicalRAMBlock {
    public:
        static inline u32 resolve(u32 address) {
            return address & 1024 * 1024 * 32 - 1;
        }
        u8* access(u32 address) {
            return &rdRamBlock[address];
        }

        // Allocating 32 megabytes of RAM to the primary CPU
        // In a simulated hardware environment, we could simply create an array of bytes to serve
        // as our RAM without any issues
        os::MappedMemory<u8> rdRamBlock{static_cast<uint64_t>(1024 * 1024 * 32)};
    };
}
