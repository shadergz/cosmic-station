#pragma once

#include <list>

#include <hle/bios_class.h>
#include <fs/bios_loader.h>

namespace zenith::hle {
    class HLEBiosGroup {
    public:
        HLEBiosGroup(JNIEnv* env)
            : android(env) {}

        bool storeAndFill(jobject model, BiosInfo&& bios);
        bool isAlreadyAdded(i32 is[2], bool usePos = false);
        bool rmFromStorage(i32 rmBy[2], bool usePos = true);
        void discardAll();
        i32 choice(i32 chBy[2], bool usePos = false);

        bool loadFrom(jobject model, i32 ldBy[2], bool usePos = false);

        std::unique_ptr<BiosInfo> systemBios;
    private:
        bool isCrucial{};
        std::list<BiosInfo> biosList;

        fs::BiosLoader loader{};
        JNIEnv* android{};
    };
}
