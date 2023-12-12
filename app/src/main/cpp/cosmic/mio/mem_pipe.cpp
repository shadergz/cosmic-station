#include <mio/mem_pipe.h>
#include <console/virt_devices.h>

namespace cosmic::mio {
    MemoryPipe::MemoryPipe(std::shared_ptr<console::VirtDevices>& devices) : devs(devices) {
        lifeFruits[0x3].off8 = devs->virtBlocks->makeRealAddress(0, mio::IopMemory);
    }
    void MemoryPipe::writeGlobal(u32 address, u32 value, u32 bitsCount) {
        std::array<u32, 4> effective{};
        effective[1] = address;
        effective[2] = value;
        effective[3] = bitsCount / 8;
        if (address >= 0x1c000000 && address < 0x1c200000) {
            effective[0] = iopDev;
            effectiveWrite(effective);
        }
        switch (address) {
        case 0x10002000:
            devs->decoderMpeg12->issueACmd(value);
        }
    }
    void MemoryPipe::effectiveWrite(std::array<u32, 4>& packet) {
        if (packet[0] == iopDev) {
            if (packet[3] == 4) {
                lifeFruits[0x3].off8 = devs->virtBlocks->iopUnaligned(packet[1]);
                *lifeFruits[0x3].off32 = packet[2];
            }
        }
    }
}