#pragma once

#include <sys/mman.h>
#include <ImplTypes.h>
namespace zenith::os {
    template<typename T>
    struct MappedMemory {
        MappedMemory<T>(T* address) : managedBlock(address) {}

        MappedMemory<T>(u64 blockSize)
            : blockRange(blockSize),
              managedBlock(reinterpret_cast<T*>(mmap(nullptr, blockRange, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0))) {}

        ~MappedMemory() {
            munmap(reinterpret_cast<T*>(managedBlock), blockRange);
        }

        static_assert(sizeof(T*) == 8, "");
        u8& operator[](u32 address) {
            return managedBlock[address];
        }
    private:
        u64 blockRange{};
        u8* managedBlock{};
    };
}
