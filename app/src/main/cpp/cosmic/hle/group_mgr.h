#pragma once

#include <list>

#include <hle/bios_class.h>
#include <fs/bios_loader.h>

namespace cosmic::hle {
    class HLEBiosGroup {
    public:
        HLEBiosGroup(JNIEnv* env);

        bool storeAndFill(jobject model, BiosInfo&& bios);
        bool isAlreadyAdded(i32 is[2], bool usePos = false);
        bool rmFromStorage(i32 rmBy[2], bool usePos = true);
        void discardAll();
        i32 choice(i32 chBy[2], bool usePos = false);

        bool loadBiosBy(jobject model, i32 ldBy[2], bool usePos = false);
        void readBios(std::span<u8> loadHere);

        std::unique_ptr<BiosInfo> slotBios;
    private:
        bool isCrucial{};
        std::list<BiosInfo> biosList;

        fs::BiosLoader loader{};
        JNIEnv* android{};
    };
}
