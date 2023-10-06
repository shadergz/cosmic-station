#pragma once

#include <list>
#include <java/jclasses.h>
namespace zenith::kernel {
    class KernelModel : java::JavaClass {
    public:
        KernelModel(JNIEnv* env)
            : java::JavaClass(env, "emu/zenith/data/KernelModel") {}
        u32 kCRCId;
        i32 kFD;
        bool selected;

        java::JNIString kObject;
        java::JNIString originVersion;
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
            if (!rIsCrucial && kernel.selected)
                rIsCrucial = true;
            kernels.push_back(kernel);
        }
    private:
        bool rIsCrucial{};
        std::list<KernelModel> kernels;
    };
}
