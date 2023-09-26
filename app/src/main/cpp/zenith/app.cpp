#include <app.h>

namespace zenith {
    std::unique_ptr<java::JvmManager> deviceRes;
    std::shared_ptr<PalePaper> userLog;
    std::unique_ptr<CoreApplication> zenithApp;
    
    CoreApplication::CoreApplication()
        : m_virtualBlocks(std::make_shared<console::GlobalMemory>()),
          m_simulatedDevices(std::make_shared<console::VirtualDevices>(m_virtualBlocks)) {

        auto osState{deviceRes->getServiceState()};
        osState.lock()->synchronizeAllSettings();
    }
}
