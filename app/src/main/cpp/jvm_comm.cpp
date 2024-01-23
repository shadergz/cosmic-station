#include <jni.h>

#include <cosmic/common/global.h>
#include <cosmic/java/device_handler.h>
#include <cosmic/java/jclasses.h>

// JNI_OnLoad  function is called when the JVM has loaded our native code in the heap, this process
// is started by Java Runtime using System.loadLibrary("cosmic")
extern "C" jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    // Kickstart the user readable log system also called as, GlobalLogger
    cosmic::user = std::make_shared<cosmic::GlobalLogger>();
    cosmic::device = std::make_unique<cosmic::java::JvmManager>(vm);

    cosmic::app = std::make_shared<cosmic::CoreApplication>();
    return JNI_VERSION_1_6;
}
extern "C"
JNIEXPORT void JNICALL
Java_emu_cosmic_MainActivity_syncSettings(JNIEnv* env, jobject thiz, jstring dateTime) {
    auto osState{cosmic::device->getStates()};
    cosmic::app->lastSetSync = cosmic::java::JniString(dateTime).get();
    osState->syncAllSettings();

    cosmic::user->success("Time of the last synchronization of global settings: {}", cosmic::app->lastSetSync);
}
