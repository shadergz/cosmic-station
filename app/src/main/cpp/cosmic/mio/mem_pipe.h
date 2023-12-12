#pragma once

#include <common/types.h>
#include <mio/dma_parallel.h>
#include <os/neon_simd.h>
namespace cosmic::console {
    class VirtDevices;
}
namespace cosmic::mio {
    union VirtualPointer {
        u32* off32;
        u8* off8;
        const char* offStr;
        VirtualPointer() = default;
        VirtualPointer(u8* addr) : off8(addr) {}
        VirtualPointer(u32* addr) : off32(addr) {}

        operator bool() {
            return off32 != nullptr;
        }
    };

    class MemoryPipe {
        static constexpr u16 iopDev{0x100};
    public:
        MemoryPipe(std::shared_ptr<console::VirtDevices>& devices);
        void writeGlobal(u32 address, os::vec128 value, u32 bitsCount);
        os::vec128 readGlobal(u32 address, u32 bitsCount);
        VirtualPointer getGlobal(u32 address, u32 dev[] = {});
        std::shared_ptr<DmaController> controller;
    private:
        std::shared_ptr<console::VirtDevices> devs;
        std::array<VirtualPointer, 4> lifeFruits{};
        VirtualPointer pointer[1];
    };
}