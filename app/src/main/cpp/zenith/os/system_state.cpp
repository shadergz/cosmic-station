#include <os/system_state.h>
namespace zenith::os {
    std::array<const std::string, 5> dsKeys{
        "dsdbAppStorage",
        "dsdbGpuTurboMode",

        "dsdbGpuCustomDriver",
        "dsdbEeMode",

        "dsdbBiosPath"
    };

    void OSMachState::syncAllSettings() {
        appStorage.updateValue();
        turboMode.updateValue();

        customDriver.updateValue();
        eeModeWay.updateValue();

        biosPath.updateValue();
    }
}
