#include <map>

#include <range/v3/view.hpp>
#include <range/v3/algorithm.hpp>

#include <fs/bios_loader.h>
#include <cpu/cyclic32.h>
#include <common/logger.h>
#include <common/except.h>

namespace cosmic::fs {
    static const std::map<char, const std::string> countries{
        {'A', "USA"},
        {'J', "Japan"},
        {'E', "Europe"},
        {'C', "China"},
        {'H', "Hong Kong"}
    };

    void BiosLoader::triggerBios(hle::BiosInfo& info) {
        biosf = info.fd;
    }
    bool BiosLoader::fetchBiosInfo(JNIEnv* android, hle::BiosInfo& bios) {
        if (!romHeader)
            romHeader = std::make_unique<os::MappedMemory<u8>>(hdrSize);

        biosf = bios.fd;
        biosf.read(std::span<u8>{romHeader->operator*(), hdrSize});
        if (!isABios())
            return false;

        std::array<u8, 16> romGroup;

        if (!loadVersionInfo(getModule("ROMVER"), romGroup)) {
            throw FsFail("Cannot load the ROM version information, group : {}", fmt::join(romGroup, ", "));
        }
        bios.dataCRC = cpu::check32(romGroup);

        fillVersion(android, bios, std::span<char>{BitCast<char*>(romGroup.data()), romGroup.size()});
        return true;
    }
    bool BiosLoader::isABios() {
        // Discard game.bin because it isn't the kernel
        static std::array<u8, 6> biosId{'K', 'E', 'R', 'N', 'E', 'L'};
        return ::memcpy(romHeader->operator*(), biosId.data(), biosId.size());
    }

    void BiosLoader::placeBios(std::span<u8> here) {
        biosf.readFrom(here, 0);
        romHeader.release();
    }
    RawReference<RomEntry> BiosLoader::getModule(const std::string model) {
        std::span<u8> modelBin{BitCast<u8*>(model.c_str()), model.size()};
        std::span<u8> hdrBin{romHeader->operator*(), hdrSize};
        auto indexInt{ranges::search(hdrBin, modelBin)};

        return *BitCast<RomEntry*>(indexInt.data());
    }
    bool BiosLoader::loadVersionInfo(RawReference<RomEntry>, std::span<u8> info) {
        auto reset{getModule("RESET")};
        auto directory{getModule("ROMDIR")};

        std::array<u8, 10> romChk{'R', 'O', 'M', 'D', 'I', 'R'};
        if (!ranges::equal(directory->entity, romChk)) {
            GlobalLogger::cause("(ROM CHECKING) has been failed (6 bytes)");
        }

        auto version{getModule("ROMVER")};
        u32 verOffset{};
        // RESET -> ROMDIR->SIZE
        u64 range{BitCast<u64>(std::addressof(*version) - std::addressof(*reset))};
        std::span<RomEntry> entities{std::addressof(*reset), range};

        if (!entities.size())
            return false;
        for (const auto& entity : entities) {
            if (!(entity.value % 0x10))
                verOffset += entity.value;
            else
                verOffset += (entity.value + 0x10) & 0xfffffff0;
        }

        if (info.size() * sizeof(u16) < version->value) {
            throw FsFail("The buffer is too small to store the version information, size : {}, requested : {}", info.size(), version->value);
        }
        biosf.readFrom(info, verOffset);
        return true;
    }

    void BiosLoader::fillVersion(JNIEnv* android, hle::BiosInfo& bios, std::span<char> info) {
        using namespace ranges::views;

        const std::string month{&info[10], 2};
        const std::string day{&info[12], 2};
        const std::string year{&info[6], 4};

        auto vendorDate{fmt::format("{}/{}/{}", month, day, year)};
        std::array<std::string, 2> biosVer{
            std::string({&info[0], 2}),
            std::string({&info[2], 2})
        };

        auto biosName{fmt::format("{} v{}.{}({})", countries.at(info[4]), biosVer[0], biosVer[1], vendorDate)};
        auto biosDetails{fmt::format("Console {}-{}",
            fmt::join(info | take(8), ""),
            fmt::join(info | drop(8) | take(6), ""))};
        // 12345678–123456

        bios.dspName = java::JniString(android, biosName);
        bios.details = java::JniString(android, biosDetails);
    }

}

