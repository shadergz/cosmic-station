#pragma once

#include <types.h>

#include <kernel/model.h>
namespace zenith::fs {
    // Discard game.bin because it isn't the kernel
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
        BiosLoader();
        ~BiosLoader();

        bool loadBios(JNIEnv* android, kernel::KernelModel& model);
        [[maybe_unused]] void loadTo(std::span<u8> here);
    private:
        bool isABios();

        RomEntry* getModule(const std::string model);
        bool loadVersionInfo(RomEntry* entry, std::span<u8> info);
        void fillVersion(JNIEnv* android, kernel::KernelModel& model, std::span<char> info);

        ZenFile biosf{};
        u8* scpRomHeader{};
    };
}
