#include <app.h>
#include <java/device_res.h>

static zenith::java::JvmManager deviceRes{};

namespace zenith {
    std::unique_ptr<CoreApplication> zenithApp;

    CoreApplication::CoreApplication()
        : m_virtualBlocks(std::make_shared<console::GlobalMemory>()),
          m_simulatedDevices(std::make_shared<console::VirtualDevices>(m_virtualBlocks)) {
        // Kickstart the user readable log system also called as, PalePaper
        userLog = std::make_shared<PalePaper>();

        auto osState{deviceRes.getServiceState()};
        osState.lock()->syncSettings();
    }
}
