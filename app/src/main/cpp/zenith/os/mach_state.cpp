#include <os/mach_state.h>

namespace zenith::os {
    std::array<const std::string, 2> statesIds{
        std::string("externalDirectory"),
        std::string("cpuExecTechnique")
    };

    void OSMachState::syncSettings() {
        externalDirectory.fetchValue();
        cpuExecutor.fetchValue();
    }
}
