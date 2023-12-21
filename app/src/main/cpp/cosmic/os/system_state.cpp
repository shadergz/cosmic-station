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
    void OsMachState::addObserver(StateId state, ObserveFunc observe) {
        if (state == AppStorage) {
            appStorage.observers.push_back(observe);
        } else if (state == GpuTurboMode) {
            turboMode.observers.push_back(observe);
        } else if (state == GpuCustomDriver) {
            customDriver.observers.push_back(observe);
        } else if (state == EeMode) {
            eeMode.observers.push_back(observe);
        } else if (state == BiosPath) {
            biosPath.observers.push_back(observe);
        } else if (state == SchedulerAffinity) {
            schedAffinity.observers.push_back(observe);
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
