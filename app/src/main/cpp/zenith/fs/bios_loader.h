#pragma once

#include <memory>

#include <types.h>

#include <kernel/model.h>
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

        bool loadBios(JNIEnv* android, kernel::KernelModel& model);
        void loadSystemBios(std::span<u8> here);
    private:
        bool isABios();

        RomEntry* getModule(const std::string model);
        bool loadVersionInfo(RomEntry* entry, std::span<u8> info);
        void fillVersion(JNIEnv* android, kernel::KernelModel& model, std::span<char> info);

        ZenFile biosf{};
        std::unique_ptr<os::MappedMemory<u8>> romHeader;
    };
}
