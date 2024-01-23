
#include <hle/bios_info.h>
#include <common/global.h>
namespace cosmic::hle {
    jobject BiosInfo::createInstance()  {
        auto ioClass{findClass()};

        auto modelInit{cosmicEnv->GetMethodID(ioClass, "<init>", "()V")};
        auto kotlinBios{cosmicEnv->NewObject(ioClass, modelInit)};

        return kotlinBios;
    }
    void BiosInfo::deleteInstance(jobject kotlinBios) {
        cosmicEnv->DeleteLocalRef(kotlinBios);
    }

    void BiosInfo::fillInstance(jobject kotlin) {
        auto kotlinModel{findClass()};

        auto posBrains{cosmicEnv->GetFieldID(kotlinModel, "position", "I")};
        auto selectedBrains{cosmicEnv->GetFieldID(kotlinModel, "selected", "Z")};
        auto biosNameBrains{cosmicEnv->GetFieldID(kotlinModel, "biosName", "Ljava/lang/String;")};
        auto biosDetailsBrains{cosmicEnv->GetFieldID(kotlinModel, "biosDetails", "Ljava/lang/String;")};

        cosmicEnv->SetIntField(kotlin, posBrains, position);

        cosmicEnv->SetBooleanField(kotlin, selectedBrains, selected);

        cosmicEnv->SetObjectField(kotlin, biosNameBrains, dspName.javaRef);
        cosmicEnv->SetObjectField(kotlin, biosDetailsBrains, details.javaRef);
    }
    void BiosInfo::chkAndLoad(i32 descriptor) {
        fd = DescriptorRaii(descriptor, true);
    }
}