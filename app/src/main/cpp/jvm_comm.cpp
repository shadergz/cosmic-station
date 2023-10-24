#include <jni.h>
#include <android/file_descriptor_jni.h>

#include <zenith/app.h>
#include <zenith/java/device_handler.h>
#include <zenith/kernel/group_mgr.h>

// JNI_OnLoad  function is called when the JVM has loaded our native code in the heap, this process
// is started by Java Runtime using System.loadLibrary("zenith")
extern "C" jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    auto desiredVersion{JNI_VERSION_1_6};
    // Kickstart the user readable log system also called as, GlobalLogger
    zenith::userLog = std::make_shared<zenith::GlobalLogger>();

    zenith::device = std::make_unique<zenith::java::JvmManager>(vm);

    zenith::zenithApp = std::make_shared<zenith::CoreApplication>();
    return desiredVersion;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_emu_zenith_helpers_KernelsHelper_addKernel(JNIEnv* env, jobject thiz, jobject descriptor, jint position) {
    zenith::kernel::KernelModel kModel{env};
    kModel.position = position;
    auto kFD{AFileDescriptor_getFd(env, descriptor)};

    auto kernels{zenith::zenithApp->getKernelsGroup()};
    auto object{kModel.createInstance()};
    zenith::i32 find[2]{kFD, 0};

    if (kernels->isAlreadyAdded(find)) {
        kernels->loadFrom(object, find, false);
        kModel.fillInstance(object);
        return object;
    }
    kModel.chkAndLoad(kFD);
    kernels->storeAndFill(object, std::move(kModel));
    return object;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_emu_zenith_helpers_KernelsHelper_removeKernel(JNIEnv* env, jobject thiz, jintArray posFd) {
    if (env->GetArrayLength(posFd) != 2) {
        throw zenith::fatalError("Not supported element array of size {} passed",
            env->GetArrayLength(posFd));
    }
    auto group{zenith::zenithApp->getKernelsGroup()};

    jint* mangled{env->GetIntArrayElements(posFd, nullptr)};

    bool hasRemoved{group->rmFromStorage(mangled)};
    env->ReleaseIntArrayElements(posFd, mangled, 0);

    return hasRemoved;
}

extern "C"
JNIEXPORT jint JNICALL
Java_emu_zenith_helpers_KernelsHelper_setKernel(JNIEnv *env, jobject thiz, jint pos) {
    auto group{zenith::zenithApp->getKernelsGroup()};
    zenith::i32 by[2]{0, pos};
    return group->choice(by, true);
}

extern "C"
JNIEXPORT jint JNICALL
Java_emu_zenith_helpers_KernelsHelper_getRunningKernel(JNIEnv* env, jobject thiz, jint defaultPos) {
    auto kernels{zenith::zenithApp->getKernelsGroup()};
    if (kernels->systemBios)
        return kernels->systemBios->position;
    return defaultPos;
}
