#include <app.h>

namespace zenith {
    std::unique_ptr<java::JvmManager> deviceRes;
    std::shared_ptr<GlobalLogger> userLog;
    std::unique_ptr<CoreApplication> zenithApp;
    
    CoreApplication::CoreApplication()
        : virtualBlocks(std::make_shared<console::GlobalMemory>()),
          simulatedDevices(std::make_shared<console::VirtualDevices>(virtualBlocks)) {

        auto osState{deviceRes->getServiceState()};
        osState.lock()->synchronizeAllSettings();
    }
}
