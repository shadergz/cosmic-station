#include <kernel/group.h>

namespace zenith::kernel {

    bool KernelsGroup::isAlreadyAdded(u32 is[2], bool useCRC) {
        bool alreadyAdded{};
        std::for_each(kernels.begin(), kernels.end(), [&is, &alreadyAdded, useCRC](const auto& kernel) {
            alreadyAdded = kernel.isSame(is, useCRC);
        });
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
        std::for_each(kernels.begin(), kernels.end(), [chBy, useCRC, &picked](auto& kernel) {
            if (kernel.isSame(chBy, useCRC)) {
                kernel.kSelected = true;
                picked = true;
            }
        });
        return picked;
    }

    bool KernelsGroup::loadFrom(jobject model, u32 ldBy[2], bool useCRC) {
        bool loaded{};
        std::for_each(kernels.begin(), kernels.end(), [model, ldBy, useCRC, &loaded](auto& kernel) {
            if (kernel.isSame(ldBy, useCRC)) {
                kernel.fillInstance(model);
                loaded = true;
            }
        });
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