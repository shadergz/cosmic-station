#include <mio/mem_pipe.h>
#include <console/virt_devices.h>

namespace cosmic::mio {
    VirtualPointer MemoryPipe::solveGlobal(u32 address, PipeAccess dev) {
        auto isAMips{dev == IopDev || dev == EngineDev};
        if (address >= 0x1fc00000 && address < 0x20000000 && isAMips) {
            return devs->virtBlocks->makeRealAddress(address - 0x1fc00000, MainMemory);
        }
        if (dev == IopDev) {
            if (address < 0x00200000)
                return devs->virtBlocks->iopUnaligned(address);
        } else if (dev == EngineDev) {
            return devs->virtBlocks->makeRealAddress(address, MainMemory);
        }
        return {};
    }
    MemoryPipe::MemoryPipe(std::shared_ptr<console::VirtDevices>& devices) : devs(devices) {
    }
    void MemoryPipe::writeGlobal(u32 address, os::vec128 value, u64 nc, PipeAccess dev) {
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
    os::vec128 MemoryPipe::readGlobal(u32 address, u64 nc, PipeAccess dev) {
        pointer[0] = solveGlobal(address, dev);
        os::vec128 result{};
        if (pointer[0]) {
            result = readBack(pointer[0], static_cast<u8>(nc));
        } else {
            if (address >= 0x10008000 && address < 0x1000f000) {
                return controller->performRead(address);
            }
        }
        return result;
    }
}