#include <java/device_handler.h>

namespace cosmic::java {
    JvmManager::JvmManager(JavaVM* jvm) :
        androidRuntime(jvm) {
        void* env{};
        androidRuntime->GetEnv(&env, JNI_VERSION_1_6);
        android = reinterpret_cast<JNIEnv*>(env);

        state = std::make_shared<os::OsMachState>(android);
    }
}
