#include <mio/mem_pipe.h>
#include <console/virt_devices.h>

namespace cosmic::mio {
    VirtualPointer MemoryPipe::getGlobal(u32 address, u32 dev[]) {
        if (dev)
            dev[0] = iopDev;
        if (address >= 0x1c000000 && address < 0x1c200000) {
            return devs->virtBlocks->iopUnaligned(address & 0x1fffff);
        }
        return {};
    }
    MemoryPipe::MemoryPipe(std::shared_ptr<console::VirtDevices>& devices) : devs(devices) {
        lifeFruits[0x3].off8 = devs->virtBlocks->makeRealAddress(0, mio::IopMemory);
    }

    void MemoryPipe::writeGlobal(u32 address, os::vec128 value, u32 bitsCount) {
        std::array<u32, 2> effective{bitsCount / 8};
        pointer[0] = getGlobal(address, &effective[1]);

        if (effective[1] == iopDev) {
            if (effective[0] == 0x4) {
                *lifeFruits[0x3].off32 = value.to32(0);
            }
        }
        switch (address) {
        case 0x10002000:
            if (effective[0] != 0x4)
                break;
            devs->decoderMpeg12->issueACmd(value.to32(0));
        }
    }
    os::vec128 MemoryPipe::readGlobal(u32 address, u32 bitsCount) {
        u32 device;
        pointer[0] = getGlobal(address, &device);
        u64 clb;
        if (bitsCount == 16)
            ;
        else if (bitsCount == 32)
            clb = 0xffffffff;

        if (device == iopDev) {
            if (bitsCount == 32)
                return (*lifeFruits[0x3].off32) & clb;
        }
        return {};
    }
}