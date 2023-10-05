#include <java/device_handler.h>

namespace zenith::java {
    std::weak_ptr<os::OSMachState> JvmManager::getServiceState() {
        return state;
    }
}
