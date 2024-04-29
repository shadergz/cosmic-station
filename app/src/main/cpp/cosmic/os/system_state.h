#pragma once

#include <array>
#include <functional>
#include <vector>
#include <cassert>

#include <java/jclasses.h>
#include <java/env.h>
#include <common/types.h>

namespace cosmic {
    extern thread_local java::CosmicEnv cosmicEnv;
}
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
        OsVariable(const std::string& stateName) {
            auto state{cosmicEnv->NewStringUTF(stateName.data())};
            varName = BitCast<jstring>(cosmicEnv->NewGlobalRef(state));
            if (varName) {
                cosmicEnv->DeleteLocalRef(state);
            }
        }
        ~OsVariable() {
            cosmicEnv->DeleteGlobalRef(varName);
        }

        void updateValue() {
            auto settingsClass{
                cosmicEnv->FindClass("emu/cosmic/data/CosmicSettings")};
            auto updateEnvMethod{
                cosmicEnv->GetStaticMethodID(settingsClass,
                "getDataStoreValue",
                "(Ljava/lang/String;)Ljava/lang/Object;")};

            if (cosmicEnv->ExceptionCheck())
                cosmicEnv->ExceptionOccurred();

            auto result{cosmicEnv->CallStaticObjectMethod(settingsClass, updateEnvMethod, varName)};
            bool isModified{false};
            std::optional<T> stateValue{};

            if constexpr (std::is_same<T, java::JniString>::value) {
                cachedState = java::JniString(BitCast<jstring>(result));
                isModified = *stateValue != *cachedState;

            } else if constexpr (std::is_same<T, java::JniInteger>::value) {
                auto getInt{cosmicEnv->GetMethodID(cosmicEnv->GetObjectClass(result),
                    "intValue", "()I")};
                stateValue = cosmicEnv->CallIntMethod(result, getInt);
                isModified = *stateValue != *cachedState;

            } else if constexpr (std::is_same<T, java::JniBool>::value) {
                assert(cosmicEnv->IsInstanceOf(result, cosmicEnv->FindClass("java/lang/Boolean")));
                auto getBool{cosmicEnv->GetMethodID(cosmicEnv->GetObjectClass(result),
                    "booleanValue", "()Z")};
                stateValue = cosmicEnv->CallBooleanMethod(result, getBool);
                isModified = *stateValue != *cachedState;
            }
            if (isModified && stateValue.has_value()) {
                if constexpr (std::is_same<T, java::JniString>::value)
                    cachedState = std::move(*stateValue);
                else
                    cachedState = *stateValue;
                for (auto& observer : observers) {
                    if (observer)
                        observer();
                }
            }
            cosmicEnv->DeleteLocalRef(result);
        }

        void operator=(const T&& variable) {
            cachedState = std::move(variable);
        }
        auto operator*() {
            if constexpr (std::is_same<T, java::JniString>::value)
                return *(cachedState.value());
            else
                return cachedState.value();
        }
        std::vector<ObserverFunc> observers{2};
    private:
        std::optional<T> cachedState{};
        jstring varName{};
    };

    class OsMachState {
    public:
        OsMachState() :
            appStorage(dsKeys.at(AppStorage)),
            turboMode(dsKeys.at(GpuTurboMode)),
            customDriver(dsKeys.at(GpuCustomDriver)),
            eeMode(dsKeys.at(EeMode)),
            biosPath(dsKeys.at(BiosPath)),
            schedAffinity(dsKeys.at(SchedulerAffinity)) {
        }
        void addObserver(StateId state, ObserverFunc&& observe);

        void syncAllSettings();
        // Directory with write permissions selected by the user
        OsVariable<java::JniString> appStorage;
        OsVariable<java::JniBool> turboMode;
        OsVariable<java::JniString> customDriver;
        OsVariable<java::JniInteger> eeMode;
        OsVariable<java::JniString> biosPath;
        OsVariable<java::JniInteger> schedAffinity;
    };
}
