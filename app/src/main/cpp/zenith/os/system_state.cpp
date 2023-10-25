#include <os/system_state.h>
namespace zenith::os {
    std::array<const std::string, 3> dsKeys{
        "DSDB_APP_STORAGE",
        "",
        "DSDB_GPU_TURBO_MODE"
    };

    void OSMachState::syncAllSettings() {
        storageDir.updateValue();
        eeMode.updateValue();
        gpuTurboMode.updateValue();
    }
}
