#include <jni.h>

#include <zenith/common/global.h>
#include <zenith/java/device_handler.h>
#include <zenith/java/jclasses.h>

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
JNIEXPORT void JNICALL
Java_emu_zenith_MainActivity_syncStateValues(JNIEnv* env, jobject thiz, jstring dateTime) {
    auto osState{zenith::device->getStates()};
    zenith::zenithApp->lastSetSync = zenith::java::JNIString(env, dateTime).readableStr;
    osState->syncAllSettings();

    zenith::userLog->success("Time of the last synchronization of global settings: {}", zenith::zenithApp->lastSetSync);

    // For debugging purposes only, we don't want this here
    zenith::zenithApp->vm->startVM();
}
