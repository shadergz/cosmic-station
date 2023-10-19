#pragma once

#include <os/host_memory.h>
namespace zenith::link {
    class BIOSBlock {
    public:
        os::MappedMemory<u8> dynEPROM{static_cast<uint64_t>(1024 * 1024 * 4)};
    };
}
