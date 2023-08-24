#pragma once

#include <array>
#include <memory>

#include <os/host_memory.h>

namespace console {
    namespace blocks {
        struct DoubleRDRAMChips {
            os::MappedMemory<uint8_t> rdMemoryBlock{static_cast<uint64_t>(1024 * 1024 * 1024 * 32)};
        };
    }

    class GlobalMemory {
    public:
        GlobalMemory()
            : memoryBlockInt()
            {}
        auto getEEMemories() {
            return memoryBlockInt.rdMemoryBlock;
        }
        blocks::DoubleRDRAMChips memoryBlockInt{};
    };
}
