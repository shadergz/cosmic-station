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
        auto virtRead(u32 address = 0) {
            return *as<T*>(address);
        }
        template<typename T>
        void virtWrite(u32 address = 0, auto value = {}) {
            *as<T*>(address) = value;
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
    T BitBashing(const os::vec& vec) {
        if constexpr (std::is_same<T, u32>::value)
            return vec.to32(0);
        if constexpr (std::is_same<T, os::vec>::value)
            return vec;
        return {};
    }
    class MemoryPipe {
    public:
        MemoryPipe(std::shared_ptr<console::VirtDevices>& devices);
        void writeGlobal(u32 address, const os::vec& value, u64 size, PipeAccess dev);
        os::vec readGlobal(u32 address, u64 size, PipeAccess dev);

        VirtualPointer solveGlobal(u32 address = 0, PipeAccess dev = CoreDevices);
        VirtualPointer iopHalLookup(u32 address);
        VirtualPointer directPointer(u32 address, PipeAccess dev);

        std::shared_ptr<DmaController> controller;
        void resetIoVariables();

        enum MemoryOrderFuncId {
            IpuRelatedAddr,
            DmaRelatedAddr,
            IopRelatedAddr,
        };
    private:
        std::shared_ptr<console::VirtDevices> devs;
        VirtualPointer pointer;

        os::vec imageDecoderGlb(u32 address, const os::vec& value, u64 size, bool ro);
        os::vec dmaAddrCollector(u32 address, const os::vec& value, u64 size, bool ro);
        os::vec iopSpecialRegs(u32 address, const os::vec& value, u64 size, bool ro);
    };

    template <typename T>
    auto PipeCraftPtr(std::shared_ptr<MemoryPipe>& pipe,
        u32 address, PipeAccess dev = CoreDevices) {
        return pipe->directPointer(address, dev).as<T>();
    }
    template <typename T>
    auto PipeRead(std::shared_ptr<MemoryPipe>& pipe,
        u32 address, PipeAccess dev = CoreDevices) {
        return pipe->readGlobal(address, sizeof(T), dev).as<T>();
    }
    template <typename T>
    void PipeWrite(std::shared_ptr<MemoryPipe>& pipe,
        u32 address, const os::vec& value, PipeAccess dev = CoreDevices) {
        pipe->writeGlobal(address, value, sizeof(T), dev);
    }
}