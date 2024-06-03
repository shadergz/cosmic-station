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
        void updateValue();

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
            // Our application's root directory, we can save everything from here
            appStorage(dsKeys.at(AppStorage)),

            // Saves the path for the user-provided custom driver
            customDriver(dsKeys.at(GpuCustomDriver)),
            // Stores the BIOS file path in the slot
            biosPath(dsKeys.at(BiosPath)),
            // Enables turbo mode for the GPU
            turboMode(dsKeys.at(GpuTurboMode)),
            // Defines the execution scheduling scheme for the components
            schedAffinity(dsKeys.at(SchedulerAffinity)),

            // Includes the EE execution mode
            eeMode(dsKeys.at(EeMode)) {

        }
        void addObserver(StateId state, ObserverFunc&& observe);
        void syncAllSettings();

        OsVariable<java::JniString> appStorage,
            customDriver,
            biosPath;

        OsVariable<java::JniBool> turboMode;

        OsVariable<java::JniInteger> schedAffinity,
            eeMode;
    };
}
