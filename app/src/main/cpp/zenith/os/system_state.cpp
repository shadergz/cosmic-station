#include <os/system_state.h>
namespace zenith::os {
    std::array<const std::string_view, 2> statesIds{
        "App Storage Directory",
        "EE Execution Technique"
    };

    void OSMachState::syncAllSettings() {
        externalDirectory.updateValue();
        cpuExecutor.updateValue();
    }
}
