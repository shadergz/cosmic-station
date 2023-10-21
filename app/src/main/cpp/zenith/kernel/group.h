#pragma once

#include <list>

#include <kernel/model.h>
#include <fs/bios_loader.h>

namespace zenith::kernel {
    class KernelsGroup {
    public:
        KernelsGroup(JNIEnv* env)
            : android(env) {}

        bool storeAndFill(jobject model, KernelModel& kernel);
        bool isAlreadyAdded(u32 is[2], bool useCRC = false);
        bool rmFromStorage(u32 rmBy[2], bool useCRC = true);
        bool choice(u32 chBy[2], bool useCRC = false);
        bool loadFrom(jobject model, u32 ldBy[2], bool useCRC = false);
    private:
        bool isCrucial{};
        std::list<KernelModel> kernels;

        fs::BiosLoader loader{};
        JNIEnv* android{};
    };
}
