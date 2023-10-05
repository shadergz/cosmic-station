#pragma once

#include <memory>

#include <zenith/os/system_state.h>
namespace zenith::java {
    class JvmManager {
    public:
        JvmManager(JavaVM* jvm)
            : androidRuntime(jvm) {
            void* env{};
            androidRuntime->GetEnv(&env, JNI_VERSION_1_6);
            deviceState = std::make_shared<os::OSMachState>(reinterpret_cast<JNIEnv*>(env));
        }
        std::weak_ptr<os::OSMachState> getServiceState();
    private:
        JavaVM* androidRuntime;
        std::shared_ptr<os::OSMachState> deviceState;
    };
}
