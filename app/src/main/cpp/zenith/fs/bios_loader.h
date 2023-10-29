#pragma once

#include <memory>

#include <common/types.h>
#include <hle/bios_class.h>
#include <os/mapped.h>

namespace zenith::fs {
#pragma pack(push, 1)
struct RomEntry {
    std::array<u8, 10> entity;
    [[maybe_unused]] u8 ext[2];
    u32 value;
};
#pragma pack(pop)

class BiosLoader {
    public:
        static constexpr u16 hdrSize{0x3000};
        BiosLoader() = default;

        bool loadBios(JNIEnv* android, hle::BiosInfo& bios);
        void placeBios(std::span<u8> here);
    private:
        bool isABios();

        RomEntry* getModule(const std::string model);
        bool loadVersionInfo(RomEntry* entry, std::span<u8> info);
        void fillVersion(JNIEnv* android, hle::BiosInfo& bios, std::span<char> info);

        ZenFile biosf{};
        std::unique_ptr<os::MappedMemory<u8>> romHeader;
    };
}
