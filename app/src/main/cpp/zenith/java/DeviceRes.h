#pragma once

#include <memory>

#include <zenith/os/State.h>
namespace zenith::java {
    class JvmManager {
    public:
        JvmManager()
            : deviceState(std::make_shared<os::OSMachState>()) {}
        std::weak_ptr<os::OSMachState> getServiceState();
    private:
        std::shared_ptr<os::OSMachState> deviceState;
    };
}
