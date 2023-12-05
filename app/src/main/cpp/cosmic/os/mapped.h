#pragma once

#include <sys/mman.h>
#include <common/types.h>
namespace cosmic::os {
    template<typename T>
    struct MappedMemory {
        MappedMemory() = default;
        MappedMemory<T>(T* address) : managedBlock(address) {}

        MappedMemory<T>(u64 mSize)
            : blockSize(mSize * sizeof(T)),
              managedBlock(reinterpret_cast<T*>(mmap(nullptr, blockSize, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0))) {}

        ~MappedMemory() {
            munmap(reinterpret_cast<T*>(managedBlock), blockSize);
        }

        static_assert(sizeof(T*) == 8, "");
        T& operator[](u32 address) {
            return managedBlock[address];
        }
        auto operator*() {
            return managedBlock;
        }
        auto getBlockSize() {
            return blockSize;
        }
    private:
        u64 blockSize{};
        T* managedBlock{};
    };
}
