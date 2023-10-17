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
    private:
        JavaVM* androidRuntime;
        std::shared_ptr<os::OSMachState> state;
    };
}
