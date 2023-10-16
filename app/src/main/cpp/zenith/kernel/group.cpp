#include <kernel/group.h>

namespace zenith::kernel {

    bool KernelsGroup::isAlreadyAdded(u32 is[2], bool useCRC) {
        bool alreadyAdded{};
        for (const auto& kernel : kernels) {
            if (alreadyAdded)
                break;
            alreadyAdded = kernel.isSame(is, useCRC);
        }
        return alreadyAdded;
    }

    bool KernelsGroup::rmFromStorage(u32 rmBy[2], bool useCRC) {
        bool hasRemoved{};
        std::remove_if(kernels.begin(), kernels.end(), [rmBy, useCRC, &hasRemoved](const auto& kernel) {
            hasRemoved = kernel.isSame(rmBy, useCRC);
            return hasRemoved;
        });
        return hasRemoved;
    }

    bool KernelsGroup::choice(u32 chBy[2], bool useCRC) {
        bool picked{};
        for (auto& kernel : kernels) {
            picked = kernel.isSame(chBy, useCRC);
            // All non-selected kernels will have their `selected` flag cleared
            kernel.kSelected = picked;
        }
        return picked;
    }

    bool KernelsGroup::loadFrom(jobject model, u32 ldBy[2], bool useCRC) {
        bool loaded{};
        auto kernel{std::find_if(kernels.begin(), kernels.end(), [ldBy, useCRC](const auto& kernel) {
            return kernel.isSame(ldBy, useCRC);
        })};

        if (kernel != kernels.end()) {
            kernel->fillInstance(model);
            loaded = true;
        }
        return loaded;
    }

    jobject KernelModel::createInstance()  {
        auto modelInit{classEnv->GetMethodID(model, "<init>", "()V")};
        auto kotlinKernel{classEnv->NewObject(model, modelInit)};

        return kotlinKernel;
    }

    void KernelModel::fillInstance(jobject kotlin) {}
    void KernelModel::chkAndLoad(i32 fd) {
        kFD = fd;
    }
}