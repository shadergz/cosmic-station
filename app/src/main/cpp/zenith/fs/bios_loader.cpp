#include <map>

#include <range/v3/view.hpp>

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
        biosf = model.kFD;

        biosf.read(std::span<u8>{scpRomHeader, hdrSize});
        if (!isABios())
            return false;

        u64 romChk{0x524F4D444952};
        auto romDir{reinterpret_cast<RomEntry*>(getModule("ROMDIR"))};

        if (::memcpy(&romDir, &romChk, 6))
            GlobalLogger::cause("!ROM CHECKING! has been failed (6 bytes)");

        // 0xd004 to scph10000.bin
        romExe = doSpace(getModule("RESET"), romDir->value);
        // start dirSize end romEXE (result: 0xf704)
        std::array<u16, 16> romGroup;

        if (!loadDirs(getModule("ROMVER"), romGroup)) {
            throw fatalError("Cannot load the ROM version information, Group {}", fmt::join(romGroup, ", "));
        }

        model.hasLoaded = true;
        model.kName = java::JNIString(android, "NAME");

        auto manuDate{fmt::format("{}/{}/{}",
            romGroup[5], romGroup[6],
            *reinterpret_cast<u32*>(&romGroup[7]))};
        auto biosModel{fmt::format("{} v{}.{}({})",
            countries.at(static_cast<char>(romGroup[2])), romGroup[0], romGroup[1], manuDate)};
        auto originModel{fmt::format("Console {}-{}",
            fmt::join(romGroup | ranges::views::take(9), ", "), fmt::join(romGroup | ranges::views::drop(9), ", "))};

        model.kObject = java::JNIString(android, biosModel);
        model.kOriginVersion = java::JNIString(android, originModel);

        return true;
    }

    bool BiosLoader::isABios() {
        static std::array<u8, 6> biosId{'K', 'E', 'R', 'N', 'E', 'L'};
        return ::memcpy(scpRomHeader + 0x1c68, biosId.data(), biosId.size());
    }

    void BiosLoader::loadTo(std::span<u8> here) {
        biosf.read(here);
    }

    RomEntry* BiosLoader::getModule(const std::string_view model) {
        if (model == "ROMVER")
            return reinterpret_cast<RomEntry*>(scpRomHeader + 0x2730);
        else if (model == "RMDIR")
            return reinterpret_cast<RomEntry*>(scpRomHeader + 0x2710);
        else if (model == "RESET")
            return reinterpret_cast<RomEntry*>(scpRomHeader + 0x2700);

        return {};
    }

    RomEntry* BiosLoader::getDir(const std::string_view model) {
        if (model == "ROMVER") {
            return reinterpret_cast<RomEntry*>(romExe + 0x2730);
        }
        return {};
    }

    bool BiosLoader::loadDirs(RomEntry *entry, std::span<u16> info) {
        auto romVer{getDir("ROMVER")};
        u32 cursor{
            static_cast<u32>((reinterpret_cast<uintptr_t>(romVer) -
            reinterpret_cast<uintptr_t>(scpRomHeader)) / sizeof(RomEntry))};

        u32 verOffset{};
        std::span<RomEntry> entities{reinterpret_cast<RomEntry*>(doSpace(romVer, 12)), cursor};

        if (entities.size())
            return false;

        for (const auto& entity : entities) {
            if (!(entity.value % 0x10))
                verOffset += entity.value;
            else
                verOffset += (entity.value + 0x10) & 0xfffffff0;
        }

        if (info.size() * sizeof(u16) < cursor) {
            throw fatalError("The buffer is too small to store the version information, size = {}, requested = {}", info.size(), cursor);
        }

        ::memcpy(info.data(), scpRomHeader + verOffset, cursor);
        return true;
    }

    u8* BiosLoader::doSpace(RomEntry* romEntry, u32 end) {
        return reinterpret_cast<u8*>(romEntry + end);
    }
}

