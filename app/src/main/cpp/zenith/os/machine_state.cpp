#include <os/machine_state.h>

namespace zenith::os {
    std::array<const std::string, 2> statesIds{
        std::string("m_externalDirectory"),
        std::string("cpuExecTechnique")
    };

    void OSMachState::synchronizeAllSettings() {
        m_externalDirectory.updateValue();
        m_cpuExecutor.updateValue();
    }
}
