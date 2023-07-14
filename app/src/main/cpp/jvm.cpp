#include <jni.h>

// JNI_OnLoad function is called when the JVM has loaded our native code in the heap, this process
// is started by Java Runtime using System.loadLibrary("zenith")
extern "C" jint JNI_OnLoad([[maybe_unused]] JavaVM* vm, [[maybe_unused]] void* reserved) {
    auto desiredVersion{JNI_VERSION_1_6};
    return desiredVersion;
}
