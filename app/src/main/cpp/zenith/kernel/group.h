#pragma once

#include <list>

#include <kernel/model.h>
#include <fs/bios_loader.h>

namespace zenith::kernel {
    class KernelsGroup {
    public:
        KernelsGroup(JNIEnv* env)
            : android(env) {}

        bool storeAndFill(jobject model, KernelModel&& kernel);
        bool isAlreadyAdded(i32 is[2], bool usePos = false);
        bool rmFromStorage(i32 rmBy[2], bool usePos = true);
        bool choice(i32 chBy[2], bool usePos = false);
        bool loadFrom(jobject model, i32 ldBy[2], bool usePos = false);
    private:
        bool isCrucial{};
        std::list<KernelModel> kernels;

        std::unique_ptr<KernelModel> systemBios;

        fs::BiosLoader loader{};
        JNIEnv* android{};
    };
}
