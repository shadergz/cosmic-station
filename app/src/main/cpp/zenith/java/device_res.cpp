#include <java/device_res.h>

namespace zenith::java {
    std::weak_ptr<os::OSMachState> JvmManager::getServiceState() {
        return m_deviceState;
    }
}
