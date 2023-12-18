#pragma once

#include <common/types.h>
#include <mio/dma_parallel.h>
#include <os/neon_simd.h>
namespace cosmic::console {
    class VirtDevices;
}
namespace cosmic::mio {
    enum PipeAccess {
        IopDev = 0x100,
        EngineDev = 0x44,
        GifDev = 0x41,
        Vu0Dev = 0x3338,
        Vu1Dev = 0x3339
    };

    union VirtualPointer {
        u8* offset;

        template<typename T>
        auto as(u32 address = 0) {
            return reinterpret_cast<T>(offset) + address;
        }
        template<typename T>
        auto read(u32 address = 0) {
            return *as<T>(address);
        }
        template<typename T>
        void write(u32 address = 0, auto value = {}) {
            *as<T>(address) = value;
        }
        VirtualPointer() = default;
        VirtualPointer(u8* addr) : offset(addr) {}

        operator bool() {
            return offset != nullptr;
        }
    };
    template<typename Type = os::vec128>
    Type bitBashing(os::vec128 vec) {
        os::vec128 clb = 0xff;
        switch (sizeof(Type) * 8) {
        case 16: clb = 0xffff; break;
        case 32: clb = 0xffffffff; break;
        case 64: clb = 0xffffffffffffffff; break;
        case 128:
            // This will clean all bits to 0
            clb = {0xffffffffffffffff, 0xffffffffffffffff};
        }
        os::vec128 cleaned{vec & clb};
        if constexpr (std::is_same<Type, u32>::value)
            return cleaned.to32(0);
        if constexpr (std::is_same<Type, os::vec128>::value)
            return cleaned;
        return {};
    }
    class MemoryPipe {
    public:
        MemoryPipe(std::shared_ptr<console::VirtDevices>& devices);
        void writeGlobal(u32 address, os::vec128 value, u64 nc, PipeAccess dev);
        os::vec128 readGlobal(u32 address, u64 nc, PipeAccess dev);
        VirtualPointer solveGlobal(u32 address = 0, PipeAccess dev = EngineDev);
        std::shared_ptr<DmaController> controller;

        os::vec128 readBack(VirtualPointer& virt, u8 bytes) {
            if (bytes == 0x4)
                return virt.read<u32*>();
            return static_cast<u32>(0);
        }
        void writeBack(VirtualPointer& virt, os::vec128 value, u8 bytes) {
            if (bytes == 0x4) {
                virt.write<u32*>(0, bitBashing<u32>(value));
            }
        }
    private:
        std::shared_ptr<console::VirtDevices> devs;
        VirtualPointer pointer[1];
    };
}