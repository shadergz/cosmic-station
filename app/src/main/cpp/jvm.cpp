#include <jni.h>

#include <zenith/app.h>

// JNI_OnLoad function is called when the JVM has loaded our native code in the heap, this process
// is started by Java Runtime using System.loadLibrary("zenith")
extern "C" jint JNI_OnLoad([[maybe_unused]] JavaVM* vm, [[maybe_unused]] void* reserved) {
    auto desiredVersion{JNI_VERSION_1_6};
    zenith::zenithApp = std::make_unique<zenith::CoreApplication>();

    return desiredVersion;
}
