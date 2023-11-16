#include <os/system_state.h>
namespace zenith::os {
    std::array<const std::string, 6> dsKeys{
        "dsdbAppStorage",
        "dsdbGpuTurboMode",
        "dsdbGpuCustomDriver",
        "dsdbEeMode",
        "dsdbBiosPath",
        "dsdbSchedAffinity"
    };

    void OSMachState::syncAllSettings() {
        appStorage.updateValue();
        turboMode.updateValue();
        customDriver.updateValue();
        eeModeWay.updateValue();
        biosPath.updateValue();
        schedAffinity.updateValue();
    }
}
