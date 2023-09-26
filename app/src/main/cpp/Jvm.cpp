#include <jni.h>

#include <zenith/App.h>
#include <zenith/java/DeviceRes.h>

// JNI_OnLoad function is called when the JVM has loaded our native code in the heap, this process
// is started by Java Runtime using System.loadLibrary("zenith")
extern "C" jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    auto desiredVersion{JNI_VERSION_1_6};
    // Kickstart the user readable log system also called as, PalePaper
    zenith::userLog = std::make_shared<zenith::PalePaper>();

    zenith::deviceRes = std::make_unique<zenith::java::JvmManager>(vm);

    zenith::zenithApp = std::make_unique<zenith::CoreApplication>();
    return desiredVersion;
}
