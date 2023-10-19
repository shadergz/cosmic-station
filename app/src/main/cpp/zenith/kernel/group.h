#pragma once

#include <list>

#include <kernel/model.h>
namespace zenith::kernel {
    class KernelsGroup {
    public:
        KernelsGroup() = default;

        void store(KernelModel&& kernel) {
            if (!rIsCrucial && kernel.kSelected)
                rIsCrucial = true;
            kernels.push_back(kernel);
        }

        bool isAlreadyAdded(u32 is[2], bool useCRC = false);
        bool rmFromStorage(u32 rmBy[2], bool useCRC = true);
        bool choice(u32 chBy[2], bool useCRC = false);
        bool loadFrom(jobject model, u32 ldBy[2], bool useCRC = false);
    private:
        bool rIsCrucial{};
        std::list<KernelModel> kernels;
    };
}
