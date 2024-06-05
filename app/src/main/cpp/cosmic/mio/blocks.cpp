#include <common/global.h>
#include <mio/blocks.h>

#include <boost/filesystem/fstream.hpp>
namespace cosmic::mio {
    u8* GlobalMemory::mapVirtAddress(u32 address, RealAddressFrom mkFrom) {
        u32 realAddress;
        u8* hostPointer{};
        [[likely]] if (mkFrom == MainMemory) {
            realAddress = address & (1024 * 1024 * 32 - 1);
            hostPointer = access(realAddress, MainMemory);
        } else if (mkFrom == BiosMemory) {
            realAddress = address & (1024 * 1024 * 4 - 1);
            hostPointer = access(realAddress, MainMemory);
        } else if (
                mkFrom == IopMemory ||
                mkFrom == Spu2Ram) {
            realAddress = address & (1024 * 1024 * 2 - 1);
            hostPointer = access(realAddress, mkFrom);
        }
        return hostPointer;
    }

    u8* GlobalMemory::iopUnaligned(u32 address) {
        // IOP can only access its own RAM or the BIOS physically
        u8* blockPtr{};
        if (address < 0x00200000)
            blockPtr = access(address, IopMemory);
        else if (address >= 0x1fc00000 && address < 0x20000000) {
            blockPtr = access(address & 0x3fffff, BiosMemory);
        }
        return blockPtr;
    }
    u8* GlobalMemory::spu2Unaligned(u32 address) {
        return access(address, Spu2Ram);
    }

    u8* GlobalMemory::access(u32 address, RealAddressFrom from) {
        switch (from) {
        case IopMemory:
            return &iopBlock[address];
        case BiosMemory:
        case MainMemory:
            return &ramBlock[address];
        case Spu2Ram:
            return &sndBlock[address];
        }
    }
    void GlobalMemory::printMemoryImage() {
        boost::filesystem::path iopFile{"IOP.bin"};
        boost::filesystem::path sndFile{"Sound.bin"};
        boost::filesystem::path ramFile{"MainRAM.bin"};

        dumpMemoryToDisk(iopFile, iopBlock);
        dumpMemoryToDisk(sndFile, sndBlock);
        dumpMemoryToDisk(ramFile, ramBlock);
    }
    struct DumpFileImage {
        u64 version;
        u64 size;
    };

    void GlobalMemory::dumpMemoryToDisk(
        boost::filesystem::path& devOutFile,
        os::MappedMemory<u8>& devBlock) {

        boost::filesystem::path storage{*states->appStorage};
        if (!boost::filesystem::exists(storage)) {

        }
        storage.append(devOutFile);
        boost::filesystem::fstream writeFile;
        const auto ioPerm{std::ios::out | std::ios::trunc | std::ios::binary};
        writeFile.open(storage, ioPerm);

        const DumpFileImage image{
            .version = 1,
            .size = devBlock.getBlockSize()
        };
        if (writeFile) {
            writeFile << image.version;
            writeFile << image.size;

            auto size{static_cast<std::streamsize>(devBlock.getBlockSize())};
            writeFile.write(reinterpret_cast<const char*>(*devBlock), size);
        }

        writeFile.close();
    }

    constexpr u64 soundMemory = 1024 * 1024 * 2;
    constexpr u64 ioMemory = 1024 * 1024 * 2;
    // Allocating 32 megabytes of RAM to the primary CPU
    constexpr u64 mainMemory = 1024 * 1024 * 32;
    GlobalMemory::GlobalMemory() {
        constexpr std::array<uintptr_t, 3> devRegionLayout{
            0,
            soundMemory,
            soundMemory + ioMemory
        };
        // A subtotal of 36MB is needed to simulate the chips
        const u64 amountRequired{soundMemory + ioMemory + mainMemory};
        static_assert(amountRequired == 36 * 1024 * 1024);
        umm = os::MappedMemory<u8>{amountRequired};
        if (!umm) {
        }
        sndBlock = os::MappedMemory<u8>{&umm[devRegionLayout[0]], soundMemory};
        iopBlock = os::MappedMemory<u8>{&umm[devRegionLayout[1]], ioMemory};
        ramBlock = os::MappedMemory<u8>{&umm[devRegionLayout[2]], mainMemory};
    }
}