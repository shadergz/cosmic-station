#include <jni.h>
#include <android/file_descriptor_jni.h>

#include <cosmic/common/global.h>
#include <cosmic/hle/group_mgr.h>

extern "C"
JNIEXPORT jobject JNICALL
Java_emu_cosmic_helpers_BiosHelper_00024Companion_addBios(JNIEnv* env, jobject thiz, jobject descriptor, jint position) {
    cosmic::hle::BiosInfo info{};
    info.position = position;

    auto biosHld{AFileDescriptor_getFd(env, descriptor)};
    auto biosMgr{cosmic::app->getBiosMgr()};
    std::array<cosmic::i32, 2> find{biosHld, 0};

    auto object{info.createInstance()};
    if (biosMgr->isAlreadyAdded(find)) {
        biosMgr->loadBiosBy(object, find, false);
        return object;
    }

    info.chkAndLoad(biosHld);
    biosMgr->storeAndFill(object, std::move(info));
    return object;
}

extern "C"
JNIEXPORT jint JNICALL
Java_emu_cosmic_helpers_BiosHelper_00024Companion_setBios(JNIEnv* env, jobject thiz, jint pos) {
    auto group{cosmic::app->getBiosMgr()};
    std::array<cosmic::i32, 2> by{0, pos};

    return group->choice(by, true);
}
extern "C"
JNIEXPORT jboolean JNICALL
Java_emu_cosmic_helpers_BiosHelper_00024Companion_removeBios(JNIEnv* env, jobject thiz, jintArray posFd) {
    if (env->GetArrayLength(posFd) != 2) {
        throw cosmic::AppErr("Not supported element array of size {} passed",
            env->GetArrayLength(posFd));
    }
    auto group{cosmic::app->getBiosMgr()};
    jint* mangled{env->GetIntArrayElements(posFd, nullptr)};
    std::array<cosmic::i32, 2> mangle{mangled[0], mangled[1]};

    bool hasRemoved{group->rmFromStorage(mangle)};

    env->ReleaseIntArrayElements(posFd, mangled, 0);
    return hasRemoved;
}
extern "C"
JNIEXPORT void JNICALL
Java_emu_cosmic_helpers_BiosHelper_00024Companion_cleanAllBios(JNIEnv* env, jobject thiz) {
    auto bgp{cosmic::app->getBiosMgr()};
    bgp->discardAll();
}
extern "C"
JNIEXPORT jint JNICALL
Java_emu_cosmic_helpers_BiosHelper_00024Companion_getBios(JNIEnv* env, jobject thiz, jint defaultPos) {
    auto biosGroup{cosmic::app->getBiosMgr()};
    if (biosGroup->slotBios)
        return biosGroup->slotBios->position;
    return defaultPos;
}
