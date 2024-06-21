#pragma once

#include <common/types.h>
#include <mio/dma_ctrl.h>
#include <os/neon_simd.h>
namespace cosmic::console {
    class VirtDevices;
}
namespace cosmic::mio {
    enum PipeAccess {
        CoreDevices,
        IopDev,
        Spu2Dev,
        Vu0Dev,
        Vu1Dev,
        GifDev,
    };

    class VirtualPointer {
    public:
        template<typename T>
        auto as(u32 address = 0) {
            return BitCast<T>(pointer + address);
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
        VirtualPointer(u8* addr) : pointer(addr) {
        }
        VirtualPointer(u32* addr) : pointer(reinterpret_cast<u8*>(addr)) {
        }

        operator bool() {
            return pointer != nullptr;
        }
    private:
        u8* pointer{};
    };
    template<typename T = os::vec>
    T bitBashing(const os::vec& vec) {
        os::vec clb{0xff};
        switch (sizeof(T) * 8) {
        case 16: clb = 0xffff; break;
        case 32: clb = 0xffffffff; break;
        case 64: clb = 0xffffffffffffffff; break;
        case 128:
            // This will clean all bits to 0
            clb = {0xffffffffffffffff, 0xffffffffffffffff};
        }
        const os::vec cleaned{vec & clb};
        if constexpr (std::is_same<T, u32>::value)
            return cleaned.to32(0);
        if constexpr (std::is_same<T, os::vec>::value)
            return cleaned;
        return {};
    }
    class MemoryPipe {
    public:
        MemoryPipe(std::shared_ptr<console::VirtDevices>& devices);
        void writeGlobal(u32 address, os::vec value, u64 size, PipeAccess dev);
        os::vec readGlobal(u32 address, u64 size, PipeAccess dev);

        VirtualPointer solveGlobal(u32 address = 0, PipeAccess dev = CoreDevices);
        VirtualPointer iopHalLookup(u32 address);
        VirtualPointer directPointer(u32 address, PipeAccess dev);

        std::shared_ptr<DmaController> controller;

        os::vec readBack(VirtualPointer& virt, u64 size) {
            if (size == sizeof(u32))
                return virt.read<u32*>();
            return static_cast<u32>(0);
        }
        void writeBack(VirtualPointer& virt, const os::vec& value, u64 size) {
            if (size == sizeof(u32)) {
                virt.write<u32*>(0, bitBashing<u32>(value));
            }
        }

        enum MemoryOrderFuncId {
            IpuRelatedAddr,
            DmaRelatedAddr
        };
    private:
        std::shared_ptr<console::VirtDevices> devs;
        VirtualPointer pointer[1];

        os::vec imageDecoderGlb(u32 address, os::vec value, u64 size, bool ro);
        os::vec dmaAddrCollector(u32 address, os::vec value, u64 size, bool ro);
    };

    template <typename T>
    [[gnu::always_inline]] auto PipeCraftPtr(
        std::shared_ptr<MemoryPipe>& mem,
        u32 address,
        PipeAccess dev = CoreDevices) {
        return mem->directPointer(address, dev).as<T>();
    }
    template <typename T>
    [[gnu::always_inline]] auto PipeRead(
        std::shared_ptr<MemoryPipe>& mem,
        u32 address,
        PipeAccess dev = CoreDevices) {
        return mem->readGlobal(address, sizeof(T), dev).as<T>();
    }
    template <typename T>
    [[gnu::always_inline]] void PipeWrite(
        std::shared_ptr<MemoryPipe>& mem,
        u32 address,
        os::vec value,
        PipeAccess dev = CoreDevices) {
        mem->writeGlobal(address, value, sizeof(T), dev);
    }
}