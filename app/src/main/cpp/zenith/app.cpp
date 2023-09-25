#include <app.h>
#include <java/device_res.h>

std::unique_ptr<zenith::java::JvmManager> deviceRes;

namespace zenith {
    std::unique_ptr<CoreApplication> zenithApp;

    CoreApplication::CoreApplication()
        : m_virtualBlocks(std::make_shared<console::GlobalMemory>()),
          m_simulatedDevices(std::make_shared<console::VirtualDevices>(m_virtualBlocks)) {

        auto osState{deviceRes->getServiceState()};
        osState.lock()->synchronizeAllSettings();
    }
}
