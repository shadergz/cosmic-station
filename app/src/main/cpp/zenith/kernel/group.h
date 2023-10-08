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
    };

    class KernelsGroup {
    public:
        KernelsGroup() = default;
        bool isAlreadyAdded(i32 check);

        void store(KernelModel&& kernel) {
            if (!rIsCrucial && kernel.kSelected)
                rIsCrucial = true;
            kernels.push_back(kernel);
        }
        bool rmFromStore(u32 rmBy[2]);
        bool choiceByCRC(u32 kernelCRC);
    private:
        bool rIsCrucial{};
        std::list<KernelModel> kernels;
    };
}
