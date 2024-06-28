#include <range/v3/algorithm.hpp>

#include <mio/mem_pipe.h>
#include <console/virt_devices.h>
#include <common/global.h>
namespace cosmic::mio {
    VirtualPointer MemoryPipe::solveGlobal(u32 address, PipeAccess dev) {
        auto isMips{dev == IopDev || dev == CoreDevices};
        VirtualPointer virtAddress{};

        if (address >= 0x1fc00000 && address < 0x20000000 && isMips) {
            virtAddress = directPointer(address, dev);
        }
        if (dev == IopDev) {
            if (address < 0x00200000)
                virtAddress = directPointer(address, dev);
            else
                virtAddress = iopHalLookup(address);
        } else if (dev == CoreDevices) {
            virtAddress = directPointer(address, dev);
        }
        return virtAddress;
    }
    MemoryPipe::MemoryPipe(std::shared_ptr<console::VirtDevices>& devices) :
        devs(devices) {
    }
    struct GlobalRangeSpecs {
        u32 starts;
        u32 ends;
        MemoryPipe::MemoryOrderFuncId function;
    };

    os::vec MemoryPipe::imageDecoderGlb(u32 address, const os::vec& value, u64 size, bool ro) {
        os::vec ipu{};

        switch (address) {
        case 0x10002000:
            if (size != sizeof(u32))
                break;
            if (ro)
                devs->decoderMpeg12->issueACmd(BitBashing<u32>(value));
        }
        return ipu;
    }
    os::vec MemoryPipe::dmaAddrCollector(u32 address, const os::vec& value, u64 size, bool ro) {
        os::vec from{};
        if (ro)
            from = controller->performRead(address);

        return from;
    }
    os::vec MemoryPipe::iopSpecialRegs(u32 address, const os::vec& value, u64 size, bool ro) {
        u64 iopTimerIndex{};
        os::vec result{};
        switch (address) {
        }
        result = devs->mipsIop->timer->performTimerAccess(address, BitBashing<u32>(value), !ro);

        return {};
    }
    std::array<GlobalRangeSpecs, 3> globalRanges{{
        {0x10002000, 0x10002030, MemoryPipe::IpuRelatedAddr},
        {0x10008000, 0x1000f000, MemoryPipe::DmaRelatedAddr},
        {0x1f801070, 0x1f801574, MemoryPipe::IopRelatedAddr},
    }};

    void MemoryPipe::writeGlobal(u32 address, const os::vec& value, u64 size, PipeAccess dev) {
        pointer = solveGlobal(address, dev);
        bool threat{};

        ranges::for_each(globalRanges, [&](auto& region) {
            if (region.starts >= address && region.ends < address) {
                switch (region.function) {
                case IpuRelatedAddr:
                    imageDecoderGlb(address, value, size, false); break;
                case DmaRelatedAddr:
                    dmaAddrCollector(address, value, size, false); break;
                case IopRelatedAddr:
                    iopSpecialRegs(address, value, size, false); break;
                }
                threat = true;
            }
        });
        if (!threat && pointer && size == sizeof(u32))
            pointer.virtWrite<u32>(0, BitBashing<u32>(value));
    }

    os::vec MemoryPipe::readGlobal(u32 address, u64 size, PipeAccess dev) {
        pointer = solveGlobal(address, dev);
        bool threat{};
        os::vec result{};

        ranges::for_each(globalRanges, [&](auto& region) {
            if (region.starts >= address && region.ends < address) {
                switch (region.function) {
                case IpuRelatedAddr:
                    result = imageDecoderGlb(address, {}, size, true);
                case DmaRelatedAddr:
                    result = dmaAddrCollector(address, {}, size, true);
                case IopRelatedAddr:
                    result = iopSpecialRegs(address, {}, size, false); break;
                }
                threat = true;
            }
        });
        if (!threat && pointer && size == sizeof(u32))
            result = pointer.virtRead<u32>();
        return result;
    }

    // https://www.psx-place.com/threads/ps2s-builtin-ps1-functions-documentation.26901/
    enum PsxMode { Psx2Only = 0, Psx1Compatibility = 0x8 };
    u32 hwIoCfg,
        sSbus,
        hole;

    void MemoryPipe::resetIoVariables() {
        hwIoCfg = Psx2Only;
        sSbus = {};
        hole = {};
    }

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
        return {};
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