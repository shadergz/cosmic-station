#include <range/v3/algorithm.hpp>

#include <mio/mem_pipe.h>
#include <console/virt_devices.h>
#include <common/global.h>
namespace cosmic::mio {
    VirtualPointer MemoryPipe::solveGlobal(u32 address, PipeAccess dev) {
        auto isMips{dev == IopDev || dev == CoreDevices};
        if (address >= 0x1fc00000 && address < 0x20000000 && isMips) {
            return directPointer(address, dev);
        }
        if (dev == IopDev) {
            if (address < 0x00200000)
                return directPointer(address, dev);
            return iopHalLookup(address);
        } else if (dev == CoreDevices) {
            return directPointer(address, dev);
        }
        return {};
    }
    MemoryPipe::MemoryPipe(std::shared_ptr<console::VirtDevices>& devices) : devs(devices) {
    }
    struct GlobalRangeSpecs {
        u32 starts;
        u32 ends;
        MemoryPipe::MemoryOrderFuncId funcId;
    };

    os::vec MemoryPipe::imageDecoderGlb(u32 address, os::vec value, u64 size, bool ro) {
        os::vec ipu{};

        switch (address) {
        case 0x10002000:
            if (size != sizeof(u32))
                break;
            if (ro)
                devs->decoderMpeg12->issueACmd(bitBashing<u32>(value));
        }
        return ipu;
    }
    os::vec MemoryPipe::dmaAddrCollector(u32 address, os::vec value, u64 size, bool ro) {
        os::vec from{};
        if (ro)
            from = controller->performRead(address);

        return from;
    }
    std::array<GlobalRangeSpecs, 2> globalRanges{{
        {0x10002000, 0x10002030, MemoryPipe::IpuRelatedAddr},
        {0x10008000, 0x1000f000, MemoryPipe::DmaRelatedAddr}
    }};

    void MemoryPipe::writeGlobal(u32 address, os::vec value, u64 size, PipeAccess dev) {
        pointer[0] = solveGlobal(address, dev);
        bool threat{};

        ranges::for_each(globalRanges, [&](auto& region) {
            if (region.starts >= address && region.ends < address) {
                switch (region.funcId) {
                case IpuRelatedAddr:
                    imageDecoderGlb(address, value, size, false); break;
                case DmaRelatedAddr:
                    dmaAddrCollector(address, value, size, false); break;
                }
                threat = true;
            }
        });

        if (!threat && pointer[0]) {
            writeBack(pointer[0], value, size);
        }
    }

    os::vec MemoryPipe::readGlobal(u32 address, u64 size, PipeAccess dev) {
        pointer[0] = solveGlobal(address, dev);
        bool threat{};
        os::vec result{};

        ranges::for_each(globalRanges, [&](auto& region) {
            if (region.starts >= address && region.ends < address) {
                switch (region.funcId) {
                case IpuRelatedAddr:
                    result = imageDecoderGlb(address, 0, size, true);
                case DmaRelatedAddr:
                    result = dmaAddrCollector(address, 0, size, true);
                }
                threat = true;
            }
        });
        if (!threat && pointer[0]) {
            result = readBack(pointer[0], size);
        }
        return result;
    }

    // https://www.psx-place.com/threads/ps2s-builtin-ps1-functions-documentation.26901/
    enum PsxMode { Psx2Only = 0, Psx1Compatibility = 0x8 };
    u32 hwIoCfg{Psx2Only};
    u32 sSbus{};
    u32 hole{};

    struct IopTimersCct {
        u32 counter, control, target;
    };
    std::array<IopTimersCct, 1> iopTimersArea {
        {{0x1f801120, 0x1f801124, 0x1f801128}}
    };

    VirtualPointer MemoryPipe::iopHalLookup(u32 address) {
        switch (address) {
        case 0x1f801010:
            user->info("(IOP) Is attempting to access the SIF2/GPU SSBUS address");
            return &sSbus;
        case 0x1f801450:
            // The IOP will test this value as follows: 'andi $t0, $t0, 8', possibly the BIOS is
            // checking if the processor supports PS1 mode
            return &hwIoCfg;
        }
        ranges::for_each(iopTimersArea, [&](const auto& tXMap) {
            if (tXMap.counter == address) {
            }
            if (tXMap.control == address) {
            }
            if (tXMap.target == address) {
            }
        });
        return &hole;
    }
    VirtualPointer MemoryPipe::directPointer(u32 address, PipeAccess dev) {
        switch (dev) {
        case IopDev:
            return controller->mapped->iopUnaligned(address);
        case Spu2Dev:
            return controller->mapped->spu2Unaligned(address);
        case CoreDevices:
        case GifDev:
        case Vu0Dev:
        case Vu1Dev:
            return controller->mapped->mapVirtAddress(address);
        }
        return {};
    }
}