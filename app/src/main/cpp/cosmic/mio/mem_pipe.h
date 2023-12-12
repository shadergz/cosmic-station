#pragma once

#include <common/types.h>
#include <mio/dma_parallel.h>
namespace cosmic::console {
    class VirtDevices;
}
namespace cosmic::mio {
    class MemoryPipe {
        static constexpr u16 iopDev{0x100};
    public:
        MemoryPipe(std::shared_ptr<console::VirtDevices>& devices);
        void writeGlobal(u32 address, u32 value, u32 bitsCount);
        std::shared_ptr<DmaController> controller;
    private:
        void effectiveWrite(std::array<u32, 4>& packet);
        std::shared_ptr<console::VirtDevices> devs;
        union Lf {
            u32* off32;
            u8* off8;
        };
        std::array<Lf, 4> lifeFruits{};
    };
}