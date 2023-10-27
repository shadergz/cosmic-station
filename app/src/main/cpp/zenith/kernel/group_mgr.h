#pragma once

#include <list>

#include <kernel/model.h>
#include <fs/bios_loader.h>

namespace zenith::kernel {
    class KernelsGroup {
    public:
        KernelsGroup(JNIEnv* env)
            : android(env) {}

        bool storeAndFill(jobject model, BiosModel&& kernel);
        bool isAlreadyAdded(i32 is[2], bool usePos = false);
        bool rmFromStorage(i32 rmBy[2], bool usePos = true);
        void discardAll();
        i32 choice(i32 chBy[2], bool usePos = false);

        bool loadFrom(jobject model, i32 ldBy[2], bool usePos = false);

        std::unique_ptr<BiosModel> systemBios;
    private:
        bool isCrucial{};
        std::list<BiosModel> kernels;

        fs::BiosLoader loader{};
        JNIEnv* android{};
    };
}
