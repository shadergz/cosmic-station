#pragma once

#include <unordered_map>
#include <functional>
#include <vector>
#include <cassert>

#include <os/jclasses.h>
#include <os/env.h>
#include <common/types.h>

namespace cosmic {
    extern thread_local os::CosmicEnv cosmicEnv;
}
namespace cosmic::os {
    enum StateId {
        AppStorage,
        GpuCustomDriver,
        BiosPath,

        GpuTurboMode,
        DumpImageAtClash,

        SchedulerAffinity,
        EeMode,
    };
    extern const std::unordered_map<StateId, std::string> dsKeys;
    using ListenFunc = std::function<void()>;

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
        std::vector<ListenFunc> listeners{2};
        void addListener(ListenFunc&& listen) {
            listeners.push_back(listen);
        }
    private:
        std::optional<T> cachedState{};
        jstring varName{};
    };

    class OsMachState {
    public:
        OsMachState();
        void syncAllSettings();

        OsVariable<java::JniString> appStorage,
            customDriver,
            biosPath;

        OsVariable<java::JniBool> turboMode, dumpImage;

        OsVariable<java::JniInteger> schedAffinity,
            eeMode;
    private:
        [[maybe_unused]] JavaVM* androidRuntime{};
    };
}
