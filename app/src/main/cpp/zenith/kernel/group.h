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

        std::string kObject;
        std::string originVersion;
        std::string kName;

        jobject createInstance() override;
        void fillInstance(jobject kotlin) override;
    };

    class KernelsGroup {
    public:
        KernelsGroup() = default;
        bool checkByDescriptor(i32 check);
    private:
        std::list<KernelModel> kernels;
    };
}
