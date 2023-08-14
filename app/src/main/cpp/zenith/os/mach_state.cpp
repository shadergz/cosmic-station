#include <os/mach_state.h>

namespace zenith::os {
    std::array<const std::string, 1> statesIds{
            std::string("externalDirectory"),
    };

    void OSMachState::syncSettings() {
        externalDirectory.fetchValue();
        cpuExecutor.fetchValue();
    }
}
