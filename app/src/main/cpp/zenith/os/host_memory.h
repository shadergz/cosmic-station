#pragma once

#include <sys/mman.h>
#include <impl_types.h>
namespace zenith::os {
    template<typename T>
    struct MappedMemory {
        MappedMemory<T>(T* address) : managedBlock(address) {}
        MappedMemory<T>(u64 blockSize)
            : blockRange(blockSize),
              managedBlock(reinterpret_cast<T*>(mmap(nullptr, blockSize, PROT_READ | PROT_WRITE, MAP_ANONYMOUS, -1, 0)))
            {}
        ~MappedMemory() {
            munmap(reinterpret_cast<T*>(managedBlock), blockRange);
        }
    private:
        u64 blockRange;
        uint8_t* managedBlock;
    };
}
