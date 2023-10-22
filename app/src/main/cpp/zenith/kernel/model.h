#pragma once

#include <java/jclasses.h>

namespace zenith::kernel {
    class KernelModel : java::JavaClass {
    public:
        KernelModel(JNIEnv* env)
            : java::JavaClass(env, "emu/zenith/data/KernelModel") {}
        u32 id,
            dataCRC;
        i32 fd;
        bool selected{false};

        java::JNIString biosName;
        java::JNIString biosDetails;

        jobject createInstance() override;
        void fillInstance(jobject kotlin) override;
        void chkAndLoad(i32 descriptor);

        bool isSame(u32 is[2], bool useCRC = false) const {
            bool equal;
            if (useCRC)
                equal = dataCRC == is[1];
            else
                equal = fd == is[0];
            return equal;
        }
    };
}
