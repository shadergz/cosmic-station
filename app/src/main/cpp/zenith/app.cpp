#include <app.h>

namespace zenith {
    std::unique_ptr<java::JvmManager> deviceRes;
    std::shared_ptr<GlobalLogger> userLog;
    std::unique_ptr<CoreApplication> zenithApp;
    
    CoreApplication::CoreApplication()
        : virBlocks(std::make_shared<link::GlobalMemory>()),
          simulated(std::make_shared<console::VirtualDevices>(virBlocks)) {

        auto osState{deviceRes->getServiceState()};
        osState.lock()->syncAllSettings();

        driver = std::make_unique<console::EmuVM>(virBlocks, simulated);
    }
}
