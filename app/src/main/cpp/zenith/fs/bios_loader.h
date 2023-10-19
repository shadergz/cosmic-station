#pragma once

#include <types.h>

#include <kernel/model.h>
namespace zenith::fs {
    // Discard game.bin because it isn't the kernel
    struct RomEntry {
        std::array<u8, 10> entity;
        [[maybe_unused]] u8 ext[2];
        u32 value;
    };

    class [[maybe_unused]] BiosLoader {
    public:
        static constexpr u16 hdrSize{0x3000};
        BiosLoader();
        ~BiosLoader();

        [[maybe_unused]] void loadKernel(JNIEnv* android, kernel::KernelModel& model);
        [[maybe_unused]] void loadTo(std::span<u8> here);
    private:
        bool isABios();

        RomEntry* getModule(const std::string_view model);
        RomEntry* getDir(const std::string_view model);

        u8* doSpace(RomEntry* romEntry, u32 end);

        bool loadDirs(RomEntry* entry, std::span<u16> info);

        u8* romExe{};

        ZenFile biosf{};
        u8* scpRomHeader{};
    };
}
