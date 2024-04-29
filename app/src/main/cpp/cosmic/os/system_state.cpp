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
    void OsMachState::addObserver(StateId state, ObserverFunc&& observer) {
        switch (state) {
        case AppStorage:
            appStorage.observers.push_back(observer); break;
        case GpuTurboMode:
            turboMode.observers.push_back(observer); break;
        case GpuCustomDriver:
            customDriver.observers.push_back(observer); break;
        case EeMode:
            eeMode.observers.push_back(observer); break;
        case BiosPath:
            biosPath.observers.push_back(observer); break;
        case SchedulerAffinity:
            schedAffinity.observers.push_back(observer); break;
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
