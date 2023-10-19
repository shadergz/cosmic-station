#pragma once

#include <java/jclasses.h>

namespace zenith::kernel {
    class KernelModel : java::JavaClass {
    public:
        KernelModel(JNIEnv* env)
                : java::JavaClass(env, "emu/zenith/data/KernelModel") {}
        u32 kID,
                kDataCRC;
        i32 kFD;
        bool kSelected;

        java::JNIString kObject;
        java::JNIString kOriginVersion;
        java::JNIString kName;

        bool hasLoaded{false};

        jobject createInstance() override;
        void fillInstance(jobject kotlin) override;

        void chkAndLoad(i32 fd);

        bool isSame(u32 is[2], bool useCRC = false) const {
            bool equal{false};
            if (useCRC)
                equal = kDataCRC == is[1];
            else
                equal = kFD == is[0];
            return equal;
        }
    };
}
