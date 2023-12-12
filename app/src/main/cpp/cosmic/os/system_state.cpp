#include <os/system_state.h>
namespace cosmic::os {
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
        eeMode.updateValue();
        biosPath.updateValue();
        // schedAffinity.updateValue();
    }
}
