#pragma once

#include <list>
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

    class KernelsGroup {
    public:
        KernelsGroup() = default;

        void store(KernelModel&& kernel) {
            if (!rIsCrucial && kernel.kSelected)
                rIsCrucial = true;
            kernels.push_back(kernel);
        }

        bool isAlreadyAdded(u32 is[2], bool useCRC = false);
        bool rmFromStorage(u32 rmBy[2], bool useCRC = true);
        bool choice(u32 chBy[2], bool useCRC = false);
        bool loadFrom(jobject model, u32 ldBy[2], bool useCRC = false);
    private:
        bool rIsCrucial{};
        std::list<KernelModel> kernels;
    };
}
