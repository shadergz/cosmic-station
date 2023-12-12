#pragma once

#include <os/mapped.h>
namespace cosmic::mio {
    class BiosBlock {
    public:
        static inline u32 resolve(u32 address) {
            return address & 1024 * 1024 * 4 - 1;
        }
        u8* access(u32 address) {
            return &dynEprom[address];
        }
        os::MappedMemory<u8> dynEprom{static_cast<uint64_t>(1024 * 1024 * 4)};
    };
}
