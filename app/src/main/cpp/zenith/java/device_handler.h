#pragma once

#include <memory>

#include <zenith/os/system_state.h>
namespace zenith::java {
    class JvmManager {
    public:
        JvmManager(JavaVM* jvm);
        auto getServiceState() {
            return state;
        }
        JavaVM* androidRuntime{};
        JNIEnv* android{};
    private:
        std::shared_ptr<os::OSMachState> state;
    };
}
