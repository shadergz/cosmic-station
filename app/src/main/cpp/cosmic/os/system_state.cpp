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
