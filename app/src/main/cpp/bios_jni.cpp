#include <jni.h>

#include <zenith/app.h>
#include <android/file_descriptor_jni.h>
#include <zenith/kernel/group_mgr.h>

extern "C"
JNIEXPORT void JNICALL
Java_emu_zenith_MainActivity_syncStateValues(JNIEnv* env, jobject thiz, jstring dateTime) {
    auto osState{zenith::device->getServiceState()};
    zenith::zenithApp->lastSetSync = zenith::java::JNIString(env, dateTime).readableStr;
    osState->syncAllSettings();

    zenith::userLog->success("Time of the last synchronization of global settings: {}", zenith::zenithApp->lastSetSync);
}
extern "C"
JNIEXPORT jobject JNICALL
Java_emu_zenith_helpers_BiosHelper_00024Companion_addBios(JNIEnv* env, jobject thiz, jobject descriptor, jint position) {
    zenith::kernel::BiosModel kModel{env};
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
JNIEXPORT jint JNICALL
Java_emu_zenith_helpers_BiosHelper_00024Companion_setBios(JNIEnv* env, jobject thiz, jint pos) {
    auto group{zenith::zenithApp->getKernelsGroup()};
    zenith::i32 by[2]{0, pos};
    return group->choice(by, true);
}
extern "C"
JNIEXPORT jboolean JNICALL
Java_emu_zenith_helpers_BiosHelper_00024Companion_removeBios(JNIEnv* env, jobject thiz, jintArray posFd) {
    if (env->GetArrayLength(posFd) != 2) {
        throw zenith::AppFail("Not supported element array of size {} passed",
            env->GetArrayLength(posFd));
    }
    auto group{zenith::zenithApp->getKernelsGroup()};

    jint* mangled{env->GetIntArrayElements(posFd, nullptr)};

    bool hasRemoved{group->rmFromStorage(mangled)};
    env->ReleaseIntArrayElements(posFd, mangled, 0);

    return hasRemoved;
}
extern "C"
JNIEXPORT void JNICALL
Java_emu_zenith_helpers_BiosHelper_00024Companion_cleanAllBios(JNIEnv* env, jobject thiz) {
    auto kgp{zenith::zenithApp->getKernelsGroup()};
    kgp->discardAll();
}
extern "C"
JNIEXPORT jint JNICALL
Java_emu_zenith_helpers_BiosHelper_00024Companion_getRunningBios(JNIEnv* env, jobject thiz, jint defaultPos) {
    auto kernels{zenith::zenithApp->getKernelsGroup()};
    if (kernels->systemBios)
        return kernels->systemBios->position;
    return defaultPos;
}
