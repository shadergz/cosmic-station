#include <os/system_state.h>
#include <common/global.h>

namespace cosmic::os {
    const std::unordered_map<StateId, std::string> dsKeys{
        {AppStorage, "dsdb_app_storage"},
        {GpuCustomDriver, "dsdb_gpu_custom_driver"},
        {BiosPath, "dsdb_bios_path"},

        {GpuTurboMode, "dsdb_gpu_turbo_mode"},
        {DumpImageAtClash, "dsdb_dump_image_at_clash"},

        {SchedulerAffinity, "dsdb_sched_affinity"},
        {EeMode, "dsdb_ee_mode"}
    };
    OsMachState::OsMachState(JavaVM* vm) :
        // Our application's root directory, we can save everything from here
        appStorage(dsKeys.at(AppStorage)),

        // Saves the path for the user-provided custom driver
        customDriver(dsKeys.at(GpuCustomDriver)),
        // Stores the BIOS file path in the slot
        biosPath(dsKeys.at(BiosPath)),
        // Enables turbo mode for the GPU
        turboMode(dsKeys.at(GpuTurboMode)),
        dumpImage(dsKeys.at(DumpImageAtClash)),
        // Defines the execution scheduling scheme for the components
        schedAffinity(dsKeys.at(SchedulerAffinity)),

        // Includes the EE execution mode
        eeMode(dsKeys.at(EeMode)), androidRuntime(vm) {

        void* env{};
        if (androidRuntime)
            androidRuntime->GetEnv(&env, JNI_VERSION_1_6);
        cosmicEnv.feedVm(BitCast<JNIEnv*>(env));
    }

    void OsMachState::syncAllSettings() {
        appStorage.updateValue();
        customDriver.updateValue();
        biosPath.updateValue();

        turboMode.updateValue();

        eeMode.updateValue();
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
            for (auto& funcListen : listeners) {
                if (funcListen)
                    funcListen();
            }
            cosmicEnv->DeleteLocalRef(result);
        }
    }
}
