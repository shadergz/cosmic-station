#include <os/system_state.h>
#include <common/global.h>
namespace cosmic::os {
    std::array<const std::string, 6> dsKeys{
        "dsdb_app_storage",

        "dsdb_gpu_turbo_mode",

        "dsdb_gpu_custom_driver",

        "dsdb_ee_mode",

        "dsdb_bios_path",
        "dsdb_sched_affinity"
    };
    void OsMachState::addObserver(StateId state, ObserverFunc observer) {
        if (state == AppStorage) {
            appStorage.observers.push_back(observer);
        } else if (state == GpuTurboMode) {
            turboMode.observers.push_back(observer);
        } else if (state == GpuCustomDriver) {
            customDriver.observers.push_back(observer);
        } else if (state == EeMode) {
            eeMode.observers.push_back(observer);
        } else if (state == BiosPath) {
            biosPath.observers.push_back(observer);
        } else if (state == SchedulerAffinity) {
            schedAffinity.observers.push_back(observer);
        }
    }

    void OsMachState::syncAllSettings() {
        appStorage.updateValue();
        turboMode.updateValue();
        customDriver.updateValue();
        eeMode.updateValue();
        biosPath.updateValue();
    }
}
