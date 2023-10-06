#include <kernel/group.h>

namespace zenith::kernel {

    bool KernelsGroup::checkByDescriptor(i32 check) {
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

    void KernelModel::fillInstance(jobject kotlin)  {
        auto modelFDField{classEnv->GetFieldID(model, "kFD", "I")};
        classEnv->SetIntField(kotlin, modelFDField, kFD);
    }
}