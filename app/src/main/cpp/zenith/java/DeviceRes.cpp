#include <java/DeviceRes.h>

namespace zenith::java {
    std::weak_ptr<os::OSMachState> JvmManager::getServiceState() {
        return deviceState;
    }
}
