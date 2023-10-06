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

    bool KernelsGroup::rmFromStore(u32 rmBy[2]) {
        bool hasRemoved{};
        std::remove_if(kernels.begin(), kernels.end(), [rmBy, &hasRemoved](const auto& kernel) {
            hasRemoved = kernel.kID == rmBy[0] && kernel.kDataCRC == rmBy[1];
            return hasRemoved;
        });
        return hasRemoved;
    }

    bool KernelsGroup::choiceByCRC(u32 kernelCRC) {
        bool picked{};
        std::for_each(kernels.begin(), kernels.end(), [kernelCRC, &picked](const auto& kValue) {
            if (kValue.kDataCRC == kernelCRC)
                picked = true;
        });
        return picked;
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