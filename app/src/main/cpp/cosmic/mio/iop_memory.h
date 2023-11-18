#pragma once

#include <os/mapped.h>
namespace cosmic::mio {
    class IOPMemory {
    public:
        static inline u32 resolve(u32 address) {
            return address & 1024 * 1024 * 2 - 1;
        }
        u8* access(u32 address) {
            return &iopBlock[address];
        }
        os::MappedMemory<u8> iopBlock{static_cast<uint64_t>(1024 * 1024 * 2)};
    };
}
