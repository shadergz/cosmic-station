#include <os/system_state.h>
namespace zenith::os {
    std::array<const std::string, 4> dsKeys{
        "dsdbAppStorage",
        "dsdbGpuTurboMode",
        "dsdbGpuCustomDriver",
        "dsdbEeMode"
    };

    void OSMachState::syncAllSettings() {
        appStorage.updateValue();
        turboMode.updateValue();
        customDriver.updateValue();
        eeModeWay.updateValue();
    }
}
