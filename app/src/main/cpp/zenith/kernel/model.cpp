
#include <kernel/model.h>
namespace zenith::kernel {
jobject KernelModel::createInstance()  {
    auto modelInit{classEnv->GetMethodID(model, "<init>", "()V")};
    auto kotlinKernel{classEnv->NewObject(model, modelInit)};

    return kotlinKernel;
}

void KernelModel::fillInstance(jobject kotlin) {
    auto idBrains{classEnv->GetFieldID(model, "id", "I")};
    auto dataCRCBrains{classEnv->GetFieldID(model, "dataCRC", "I")};
    auto selectedBrains{classEnv->GetFieldID(model, "selected", "Z")};
    auto biosNameBrains{classEnv->GetFieldID(model, "biosName", "Ljava/lang/String;")};
    auto biosDetailsBrains{classEnv->GetFieldID(model, "biosDetails", "Ljava/lang/String;")};

    classEnv->SetIntField(kotlin, idBrains, bit_cast<jint>(id));
    classEnv->SetIntField(kotlin, dataCRCBrains, bit_cast<jint>(dataCRC));

    classEnv->SetBooleanField(kotlin, selectedBrains, static_cast<jboolean>(selected));

    classEnv->SetObjectField(kotlin, biosNameBrains, biosName.managedJava);
    classEnv->SetObjectField(kotlin, biosDetailsBrains, biosDetails.managedJava);
}
void KernelModel::chkAndLoad(i32 descriptor) {
    fd = descriptor;
}
}