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
    using ObserverFunc = std::function<void()>;

    template <typename T>
    class OsVariable {
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
            if constexpr (std::is_same<T, java::JniString>::value)
                return cachedState.operator*();
            else
                return cachedState;
        }
        void updateValue();

        std::vector<ObserverFunc> observers{2};
    private:
        JNIEnv* osEnv;
        T cachedState;
        jstring varName;

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

        if constexpr (std::is_same<T, java::JniString>::value) {
            stateValue = java::JniString(osEnv, BitCast<jstring>(result));
            isModified = stateValue != cachedState;
        } else if constexpr (std::is_same<T, java::JniInteger>::value) {
            auto getInt{osEnv->GetMethodID(osEnv->GetObjectClass(result), "intValue", "()I")};
            stateValue = osEnv->CallIntMethod(result, getInt);
            isModified = stateValue != cachedState;
        } else if constexpr (std::is_same<T, java::JniBool>::value) {
            assert(osEnv->IsInstanceOf(result, osEnv->FindClass("java/lang/Boolean")));
            auto getBool{osEnv->GetMethodID(osEnv->GetObjectClass(result), "booleanValue", "()Z")};
            stateValue = osEnv->CallBooleanMethod(result, getBool);
            isModified = stateValue != cachedState;
        }

        if (isModified) {
            if constexpr (std::is_same<T, java::JniString>::value)
                cachedState = std::move(stateValue);
            else
                cachedState = stateValue;
            for (auto& observer : observers) {
                if (observer)
                    observer();
            }
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
        void addObserver(StateId state, ObserverFunc observe);

        void syncAllSettings();
        // Directory with write permissions kSelected by the user
        OsVariable<java::JniString> appStorage;
        OsVariable<java::JniBool> turboMode;
        OsVariable<java::JniString> customDriver;
        OsVariable<java::JniInteger> eeMode;
        OsVariable<java::JniString> biosPath;
        OsVariable<java::JniInteger> schedAffinity;
    };
}
