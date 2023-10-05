#include <app.h>

namespace zenith {
    std::unique_ptr<java::JvmManager> device;
    std::shared_ptr<GlobalLogger> userLog;
    std::unique_ptr<CoreApplication> zenithApp;
    
    CoreApplication::CoreApplication()
        : virBlocks(std::make_shared<link::GlobalMemory>()),
          simulated(std::make_shared<console::VirtualDevices>(virBlocks)) {

        auto osState{device->getServiceState()};
        osState.lock()->syncAllSettings();

        driver = std::make_unique<console::EmuVM>(virBlocks, simulated);
        driver->resetVM();
    }
}
