#pragma once

#include <memory>

#include <os/system_state.h>
namespace cosmic::java {
    class JvmManager {
    public:
        JvmManager(JavaVM* jvm);
        auto getStates() {
            return state;
        }
        JavaVM* androidRuntime{};
    private:
        std::shared_ptr<os::OsMachState> state;
    };
}
