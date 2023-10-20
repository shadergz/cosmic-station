#include <map>

#include <range/v3/view.hpp>
#include <range/v3/algorithm.hpp>

#include <fs/bios_loader.h>
#include <logger.h>

namespace zenith::fs {
    static const std::map<char, const std::string> countries{
        {'A', "USA"},
        {'J', "Japan"},
        {'E', "Europe"},
        {'C', "China"},
        {'H', "Hong Kong"}
    };

    BiosLoader::BiosLoader() {

        scpRomHeader = new u8[hdrSize];
    }

    BiosLoader::~BiosLoader() {
        delete[] scpRomHeader;
    }

    bool BiosLoader::loadBios(JNIEnv* android, kernel::KernelModel &model) {
        if (!scpRomHeader)
            return false;
        biosf = model.fd;

        biosf.read(std::span<u8>{scpRomHeader, hdrSize});
        if (!isABios())
            return false;

        std::array<u8, 16> romGroup;
        if (!loadVersionInfo(getModule("ROMVER"), romGroup)) {
            throw fatalError("Cannot load the ROM version information, Group {}", fmt::join(romGroup, ", "));
        }
        fillVersion(android, model, std::span<char>{bit_cast<char*>(romGroup.data()), romGroup.size()});
        return true;
    }

    bool BiosLoader::isABios() {
        static std::array<u8, 6> biosId{'K', 'E', 'R', 'N', 'E', 'L'};
        return ::memcpy(scpRomHeader + 0x1c68, biosId.data(), biosId.size());
    }

    void BiosLoader::loadTo(std::span<u8> here) {
        biosf.read(here);
    }

    RomEntry* BiosLoader::getModule(const std::string model) {
        std::span<u8> modelBin{bit_cast<u8*>(model.c_str()), model.size()};
        std::span<u8> hdrBin{scpRomHeader, hdrSize};
        auto indexInt{ranges::search(hdrBin, modelBin)};

        return bit_cast<RomEntry*>(indexInt.data());
    }

    bool BiosLoader::loadVersionInfo(RomEntry* entry, std::span<u8> info) {
        auto reset{reinterpret_cast<RomEntry*>(getModule("RESET"))};
        auto directory{reinterpret_cast<RomEntry*>(getModule("ROMDIR"))};

        std::array<u8, 10> romChk{'R', 'O', 'M', 'D', 'I', 'R'};
        if (!ranges::equal(directory->entity, romChk)) {
            GlobalLogger::cause("(ROM CHECKING) has been failed (6 bytes)");
        }

        auto version{getModule("ROMVER")};
        u32 verOffset{};
        // RESET -> ROMDIR->SIZE
        std::span<RomEntry> entities{reset,  bit_cast<u64>(version - reset)};

        if (!entities.size())
            return false;
        for (const auto& entity : entities) {
            if (!(entity.value % 0x10))
                verOffset += entity.value;
            else
                verOffset += (entity.value + 0x10) & 0xfffffff0;
        }

        if (info.size() * sizeof(u16) < version->value) {
            throw fatalError("The buffer is too small to store the version information, size = {}, requested = {}", info.size(), version->value);
        }
        biosf.readFrom(info, verOffset);
        return true;
    }

    void BiosLoader::fillVersion(JNIEnv *android, kernel::KernelModel &model, std::span<char> info) {
        using namespace ranges::views;

        const std::string month{&info[10], 2};
        const std::string day{&info[12], 2};
        const std::string year{&info[6], 4};

        auto manuDate{fmt::format("{}/{}/{}", month, day, year)};
        const std::string biosVerP1{&info[0], 2};
        const std::string biosVerP2{&info[2], 2};

        auto biosModel{fmt::format("{} v{}.{}({})", countries.at(info[4]), biosVerP1, biosVerP2, manuDate)};

        auto originModel{fmt::format("Console {}-{}",
            fmt::join(info | take(9), ""),
            fmt::join(info | drop(9) | take(5), ""))};

        model.biosName = java::JNIString(android, biosModel);
        model.biosDetails = java::JNIString(android, originModel);
    }

}
