#include <os/system_state.h>
namespace zenith::os {
    std::array<const std::string, 2> statesIds{
        std::string("App Working Directory"),
        std::string("EE Execution Technique")
    };

    void OSMachState::syncAllSettings() {
        externalDirectory.updateValue();
        cpuExecutor.updateValue();
    }
}
