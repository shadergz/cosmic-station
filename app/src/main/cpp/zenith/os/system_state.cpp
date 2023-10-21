#include <os/system_state.h>
namespace zenith::os {
    std::array<const std::string, 2> statesIds{
        "App Storage Directory",
        "EE Execution Mode"
    };

    void OSMachState::syncAllSettings() {
        storageDir.updateValue();
        eeMode.updateValue();
    }
}
