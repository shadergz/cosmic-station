#include <jni.h>
#include <android/file_descriptor_jni.h>

#include <zenith/common/global.h>
#include <zenith/hle/group_mgr.h>

extern "C"
JNIEXPORT jobject JNICALL
Java_emu_zenith_helpers_BiosHelperModel_00024Companion_addBios(JNIEnv* env, jobject thiz, jobject descriptor, jint position) {
    zenith::hle::BiosInfo info{env};
    info.position = position;
    auto biosHld{AFileDescriptor_getFd(env, descriptor)};

    auto biosMgr{zenith::zenithApp->getBiosMgr()};
    auto object{info.createInstance()};
    zenith::i32 find[2]{biosHld, 0};

    if (biosMgr->isAlreadyAdded(find)) {
        biosMgr->loadBiosBy(object, find, false);
        info.fillInstance(object);
        return object;
    }

    info.chkAndLoad(biosHld);
    biosMgr->storeAndFill(object, std::move(info));
    return object;
}
extern "C"
JNIEXPORT jint JNICALL
Java_emu_zenith_helpers_BiosHelperModel_00024Companion_setBios(JNIEnv* env, jobject thiz, jint pos) {
    auto group{zenith::zenithApp->getBiosMgr()};
    zenith::i32 by[2]{0, pos};
    return group->choice(by, true);
}
extern "C"
JNIEXPORT jboolean JNICALL
Java_emu_zenith_helpers_BiosHelperModel_00024Companion_removeBios(JNIEnv* env, jobject thiz, jintArray posFd) {
    if (env->GetArrayLength(posFd) != 2) {
        throw zenith::AppFail("Not supported element array of size {} passed",
            env->GetArrayLength(posFd));
    }
    auto group{zenith::zenithApp->getBiosMgr()};

    jint* mangled{env->GetIntArrayElements(posFd, nullptr)};

    bool hasRemoved{group->rmFromStorage(mangled)};
    env->ReleaseIntArrayElements(posFd, mangled, 0);

    return hasRemoved;
}
extern "C"
JNIEXPORT void JNICALL
Java_emu_zenith_helpers_BiosHelperModel_00024Companion_cleanAllBios(JNIEnv* env, jobject thiz) {
    auto bgp{zenith::zenithApp->getBiosMgr()};
    bgp->discardAll();
}
extern "C"
JNIEXPORT jint JNICALL
Java_emu_zenith_helpers_BiosHelperModel_00024Companion_getBios(JNIEnv* env, jobject thiz, jint defaultPos) {
    auto biosGroup{zenith::zenithApp->getBiosMgr()};
    if (biosGroup->slotBios)
        return biosGroup->slotBios->position;
    return defaultPos;
}
