#pragma once

#include <os/host_memory.h>

namespace zenith::console {
    class GlobalMemory {
    public:
        auto getEEMemories() {
            return mainMemoryBlock;
        }
        os::MappedMemory<u8> mainMemoryBlock{static_cast<uint64_t>(1024 * 1024 * 1024 * 32)};
    };
}
