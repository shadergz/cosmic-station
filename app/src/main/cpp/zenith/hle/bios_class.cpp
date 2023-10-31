
#include <hle/bios_class.h>
namespace zenith::hle {
    jobject BiosInfo::createInstance()  {
        auto ioClass{findClass()};

        auto modelInit{classEnv->GetMethodID(ioClass, "<init>", "()V")};
        auto kotlinBios{classEnv->NewObject(ioClass, modelInit)};

        return kotlinBios;
    }

    void BiosInfo::fillInstance(jobject kotlin) {
        auto kotlinModel{findClass()};

        auto posBrains{classEnv->GetFieldID(kotlinModel, "position", "I")};
        auto selectedBrains{classEnv->GetFieldID(kotlinModel, "selected", "Z")};
        auto biosNameBrains{classEnv->GetFieldID(kotlinModel, "biosName", "Ljava/lang/String;")};
        auto biosDetailsBrains{classEnv->GetFieldID(kotlinModel, "biosDetails", "Ljava/lang/String;")};

        classEnv->SetIntField(kotlin, posBrains, position);

        classEnv->SetBooleanField(kotlin, selectedBrains, selected);

        classEnv->SetObjectField(kotlin, biosNameBrains, dspName.javaRef);
        classEnv->SetObjectField(kotlin, biosDetailsBrains, details.javaRef);
    }
    void BiosInfo::chkAndLoad(i32 descriptor) {
        fd = ZenFile(descriptor, true);
    }
}