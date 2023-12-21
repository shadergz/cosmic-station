#pragma once

#include <array>
#include <functional>
#include <vector>
#include <cassert>

#include <java/jclasses.h>
#include <common/types.h>
namespace cosmic::os {
    enum StateId : u16 {
        AppStorage,
        GpuTurboMode,
        GpuCustomDriver,
        EeMode,
        BiosPath,
        SchedulerAffinity
    };
    extern std::array<const std::string, 6> dsKeys;
    using ObserveFunc = std::function<void(JNIEnv*)>;

    template <typename T>
    struct OsVariable {
    public:
        OsVariable<T>(JNIEnv* androidEnv, const std::string& stateName) :
            osEnv(androidEnv), cachedState() {

            auto state{osEnv->NewStringUTF(stateName.data())};
            varName = static_cast<jstring>(osEnv->NewGlobalRef(state));
            osEnv->DeleteLocalRef(state);
        }
        ~OsVariable() {
            osEnv->DeleteGlobalRef(varName);
        }
        void operator=(const T&& variable) {
            cachedState = variable;
        }
        auto operator*() {
            if constexpr (std::is_same<T, java::JNIString>::value)
                return cachedState.operator*();
            else
                return cachedState;
        }
        void updateValue();

        JNIEnv* osEnv;
        T cachedState;
        jstring varName;

        std::vector<ObserveFunc> observers{2};
    };

    template<typename T>
    void OsVariable<T>::updateValue() {
        auto settingsClass{osEnv->FindClass("emu/cosmic/data/CosmicSettings")};
        auto updateEnvMethod{osEnv->GetStaticMethodID(settingsClass, "getDataStoreValue", "(Ljava/lang/String;)Ljava/lang/Object;")};

        if (osEnv->ExceptionCheck())
            osEnv->ExceptionOccurred();

        auto result{osEnv->CallStaticObjectMethod(settingsClass, updateEnvMethod, varName)};
        bool isModified{false};
        T stateValue;

        if constexpr (std::is_same<T, java::JNIString>::value) {
            stateValue = java::JNIString(osEnv, bit_cast<jstring>(result));
            isModified = stateValue != cachedState;
        } else if constexpr (std::is_same<T, java::JNIInteger>::value) {
            auto getInt{osEnv->GetMethodID(osEnv->GetObjectClass(result), "intValue", "()I")};
            stateValue = osEnv->CallIntMethod(result, getInt);
            isModified = stateValue != cachedState;
        } else if constexpr (std::is_same<T, java::JNIBool>::value) {
            assert(osEnv->IsInstanceOf(result, osEnv->FindClass("java/lang/Boolean")));
            auto getBool{osEnv->GetMethodID(osEnv->GetObjectClass(result), "booleanValue", "()Z")};
            stateValue = osEnv->CallBooleanMethod(result, getBool);
            isModified = stateValue != cachedState;
        }

        if (isModified) {
            if constexpr (std::is_same<T, java::JNIString>::value)
                cachedState = std::move(stateValue);
            else
                cachedState = stateValue;
            for (auto& observe : observers)
                if (observe)
                    observe(osEnv);
        }
        osEnv->DeleteLocalRef(result);
    }

    class OsMachState {
    public:
        OsMachState(JNIEnv* androidEnv) :
            appStorage(androidEnv, dsKeys.at(AppStorage)),
            turboMode(androidEnv, dsKeys.at(GpuTurboMode)),
            customDriver(androidEnv, dsKeys.at(GpuCustomDriver)),
            eeMode(androidEnv, dsKeys.at(EeMode)),
            biosPath(androidEnv, dsKeys.at(BiosPath)),
            schedAffinity(androidEnv, dsKeys.at(SchedulerAffinity)) {
        }
        void addObserver(StateId state, ObserveFunc observe);

        void syncAllSettings();
        // Directory with write permissions kSelected by the user
        OsVariable<java::JNIString> appStorage;
        OsVariable<java::JNIBool> turboMode;
        OsVariable<java::JNIString> customDriver;
        OsVariable<java::JNIInteger> eeMode;
        OsVariable<java::JNIString> biosPath;
        OsVariable<java::JNIInteger> schedAffinity;
    };
}
