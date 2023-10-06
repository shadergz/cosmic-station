#include <jni.h>
#include <android/file_descriptor_jni.h>

#include <zenith/app.h>
#include <zenith/java/device_handler.h>
#include <zenith/kernel/group.h>

// JNI_OnLoad function is called when the JVM has loaded our native code in the heap, this process
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
Java_emu_zenith_helper_KernelsHelper_kernelAdd(JNIEnv *env, jobject thiz, jobject descriptor) {
    zenith::kernel::KernelModel kModel{env};
    auto kFD{AFileDescriptor_getFd(env, descriptor)};

    auto kernels{zenith::zenithApp->getKernelsGroup()};
    auto object{kModel.createInstance()};

    if (kernels->isAlreadyAdded(kFD))
        return object;

    kModel.chkAndLoad(kFD);
    kModel.fillInstance(object);

    kernels->store(std::move(kModel));
    return object;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_emu_zenith_helper_KernelsHelper_kernelRemove(JNIEnv *env, jobject thiz, jlongArray kFDwCRC) {
    if (env->GetArrayLength(kFDwCRC) != 2) {
        throw zenith::fatalError("Not supported element array size {} passed to KernelSet",
            env->GetArrayLength(kFDwCRC));
    }
    auto group{zenith::zenithApp->getKernelsGroup()};

    jlong* mangled{env->GetLongArrayElements(kFDwCRC, nullptr)};
    zenith::u32 downVote[2];
    downVote[0] = static_cast<zenith::u32>(mangled[0]);
    downVote[1] = static_cast<zenith::u32>(mangled[1]);

    bool hasRemoved{group->rmFromStore(downVote)};
    env->ReleaseLongArrayElements(kFDwCRC, mangled, 0);

    return hasRemoved;
}
extern "C"
JNIEXPORT jboolean JNICALL
Java_emu_zenith_helper_KernelsHelper_kernelSet(JNIEnv *env, jobject thiz, jlong kCRC) {
    auto group{zenith::zenithApp->getKernelsGroup()};
    return group->choiceByCRC(static_cast<zenith::u32>(kCRC));
}
