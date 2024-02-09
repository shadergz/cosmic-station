#pragma once

#include <list>

#include <hle/bios_info.h>
#include <fs/bios_loader.h>
namespace cosmic::hle {
    class HleBiosGroup {
    public:
        HleBiosGroup();

        bool storeAndFill(jobject model, BiosInfo&& bios);
        bool isAlreadyAdded(std::array<i32, 2>& is, bool usePos = false);
        bool rmFromStorage(std::array<i32, 2>& rmBy, bool usePos = true);
        void discardAll();
        i32 choice(std::array<i32, 2>& chBy, bool usePos = false);

        bool loadBiosBy(jobject model, std::array<i32, 2>& ldBy, bool usePos = false);
        void readBios(std::span<u8> loadHere);

        std::unique_ptr<BiosInfo> slotBios;
    private:
        bool isCrucial{};
        std::list<BiosInfo> biosList;

        fs::BiosLoader loader{};
    };
}
