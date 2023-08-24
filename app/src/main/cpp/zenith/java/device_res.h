#pragma once

#include <memory>

#include <zenith/os/mach_state.h>

namespace zenith::java {
    class JvmManager {
    public:
        JvmManager()
            : m_deviceState(std::make_shared<os::OSMachState>())
            {}
        std::weak_ptr<os::OSMachState> getOSState();
    private:
        std::shared_ptr<os::OSMachState> m_deviceState;
    };
}
