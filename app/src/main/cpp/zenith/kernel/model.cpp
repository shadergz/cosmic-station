
#include <kernel/model.h>
namespace zenith::kernel {
    jobject KernelModel::createInstance()  {
        auto kotlinModel{findClass()};

        auto modelInit{classEnv->GetMethodID(kotlinModel, "<init>", "()V")};
        auto kotlinKernel{classEnv->NewObject(kotlinModel, modelInit)};

        return kotlinKernel;
    }

    void KernelModel::fillInstance(jobject kotlin) {
        auto kotlinModel{findClass()};

        auto idBrains{classEnv->GetFieldID(kotlinModel, "id", "I")};
        auto dataCRCBrains{classEnv->GetFieldID(kotlinModel, "dataCRC", "I")};
        auto selectedBrains{classEnv->GetFieldID(kotlinModel, "selected", "Z")};
        auto biosNameBrains{classEnv->GetFieldID(kotlinModel, "biosName", "Ljava/lang/String;")};
        auto biosDetailsBrains{classEnv->GetFieldID(kotlinModel, "biosDetails", "Ljava/lang/String;")};

        classEnv->SetIntField(kotlin, idBrains, bit_cast<jint>(id));
        classEnv->SetIntField(kotlin, dataCRCBrains, bit_cast<jint>(dataCRC));

        classEnv->SetBooleanField(kotlin, selectedBrains, static_cast<jboolean>(selected));

        classEnv->SetObjectField(kotlin, biosNameBrains, biosName.javaRef);
        classEnv->SetObjectField(kotlin, biosDetailsBrains, biosDetails.javaRef);
    }
    void KernelModel::chkAndLoad(i32 descriptor) {
        fd = descriptor;
    }
}