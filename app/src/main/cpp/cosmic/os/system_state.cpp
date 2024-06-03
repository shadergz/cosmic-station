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
    template <typename T>
    void OsVariable<T>::updateValue() {
        auto settingsClass{cosmicEnv->FindClass("emu/cosmic/data/CosmicSettings")};
        auto updateEnvMethod{
            cosmicEnv->GetStaticMethodID(settingsClass,
                "getDataStoreValue", "(Ljava/lang/String;)Ljava/lang/Object;")};
        if (cosmicEnv->ExceptionCheck()) {
            cosmicEnv->ExceptionOccurred();
        }

        auto result{cosmicEnv->CallStaticObjectMethod(settingsClass, updateEnvMethod, varName)};
        bool isModified{false};
        std::optional<T> stateValue{};

        if constexpr (std::is_same<T, java::JniString>::value) {
            stateValue = java::JniString(BitCast<jstring>(result));
        }

        if constexpr (std::is_same<T, java::JniInteger>::value) {
            auto getInt{cosmicEnv->GetMethodID(cosmicEnv->GetObjectClass(result), "intValue", "()I")};
            stateValue = cosmicEnv->CallIntMethod(result, getInt);
        }
        if constexpr (std::is_same<T, java::JniBool>::value) {
            assert(cosmicEnv->IsInstanceOf(result, cosmicEnv->FindClass("java/lang/Boolean")));
            auto getBool{cosmicEnv->GetMethodID(cosmicEnv->GetObjectClass(result), "booleanValue", "()Z")};
            stateValue = cosmicEnv->CallBooleanMethod(result, getBool);
        }

        isModified = *stateValue != *cachedState;
        if (isModified && stateValue.has_value()) {
            if constexpr (std::is_same<T, java::JniString>::value)
                cachedState = std::move(*stateValue);
            else
                cachedState = *stateValue;
            for (auto& observer : observers) {
                if (observer)
                    observer();
            }
            cosmicEnv->DeleteLocalRef(result);
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
