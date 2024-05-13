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
    void MemoryPipe::writeGlobal(u32 address, os::vec value, u64 nc, PipeAccess dev) {
        std::array<u32, 2> effective{static_cast<u32>(nc), 0};
        pointer[0] = solveGlobal(address, dev);
        writeBack(pointer[0], value, static_cast<u8>(nc));

        switch (address) {
        case 0x10002000:
            if (effective[0] != 0x4)
                break;
            devs->decoderMpeg12->issueACmd(bitBashing<u32>(value));
        }
    }
    os::vec MemoryPipe::readGlobal(u32 address, u64 nc, PipeAccess dev) {
        pointer[0] = solveGlobal(address, dev);
        os::vec result{};
        if (pointer[0]) {
            result = readBack(pointer[0], static_cast<u8>(nc));
        } else {
            if (address >= 0x10008000 && address < 0x1000f000) {
                return controller->performRead(address);
            }
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
        {{0x1F801120, 0x1F801124, 0x1F801128}}
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
        ranges::for_each(iopTimersArea, [&](auto& tXMap) {
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