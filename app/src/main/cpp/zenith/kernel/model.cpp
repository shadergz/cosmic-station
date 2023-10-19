
#include <kernel/model.h>
namespace zenith::kernel {
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