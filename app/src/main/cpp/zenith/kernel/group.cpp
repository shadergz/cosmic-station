#include <kernel/group.h>

namespace zenith::kernel {

    bool KernelsGroup::isAlreadyAdded(i32 check) {
        bool alreadyAdded{};
        std::for_each(kernels.begin(), kernels.end(), [check, &alreadyAdded](const auto& kValue) {
            if (alreadyAdded)
                return;
            if (kValue.kFD == check)
                alreadyAdded = true;
        });
        return alreadyAdded;
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