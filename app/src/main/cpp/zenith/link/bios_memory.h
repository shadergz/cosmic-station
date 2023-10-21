#pragma once

#include <os/mapped.h>
namespace zenith::link {
    class BIOSBlock {
    public:
        static inline u32 resolve(u32 address) {
            return address &= 1024 * 1024 * 4 - 1;
        }
        u8* access(u32 address) {
            return &dynEPROM[address];
        }
        os::MappedMemory<u8> dynEPROM{static_cast<uint64_t>(1024 * 1024 * 4)};
    };
}
