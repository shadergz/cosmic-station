#pragma once

#include <os/host_memory.h>

namespace zenith::console {
    class GlobalMemory {
    public:
        auto getEEMemories() {
            return mainMemoryBlock;
        }
        os::MappedMemory<uint8_t> mainMemoryBlock{static_cast<uint64_t>(1024 * 1024 * 1024 * 32)};
    };
}
