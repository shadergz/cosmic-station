#include "app.h"
#include "java/device_mgr.h"

zenith::java::JvmManager deviceRes{};

namespace zenith {
    [[maybe_unused]] std::unique_ptr<CoreApplication> zenithApp;

    CoreApplication::CoreApplication() {
        auto osState{deviceRes.getOSState()};
        osState.lock()->downloadSettings();
    }
}
