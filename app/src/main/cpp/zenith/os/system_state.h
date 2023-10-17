#pragma once

#include <array>

#include <java/jclasses.h>
#include <types.h>
namespace zenith::os {
    enum StateIDs {
        appStorageDir,
        eeExecTechnique
    };
    extern std::array<const std::string_view, 2> statesIds;

    template <typename T>
    struct OSVariable {
    public:
        OSVariable<T>(JNIEnv* androidEnv, const std::string_view& stateName)
            : osEnv(androidEnv), cachedVar() {
            varName = osEnv->NewStringUTF(stateName.data());
        }
        ~OSVariable() {
            osEnv->DeleteLocalRef(varName);
        }
        void operator=(const T&& variable) {
            cachedVar = variable;
        }
        auto operator*() {
            return cachedVar;
        }
        void updateValue();

        JNIEnv* osEnv;
        T cachedVar;
        jstring varName;
    };

    template<typename T>
    void OSVariable<T>::updateValue() {
        auto settingsClass{osEnv->FindClass("emu/zenith/data/ZenithSettings")};
        auto updateEnvMethod{osEnv->GetStaticMethodID(settingsClass,
            "getEnvStateVar", "(Ljava/lang/String;)Ljava/lang/Object;")};
        auto result{osEnv->CallStaticObjectMethod(settingsClass, updateEnvMethod, varName)};

        if constexpr (std::is_same<T, java::JNIString>::value) {
            cachedVar = java::JNIString(osEnv, static_cast<jstring>(result));
        } else if constexpr (std::is_same<T, java::JNIEnumerator>::value) {
            auto getInt{osEnv->GetMethodID(osEnv->GetObjectClass(result), "intValue", "()I")};
            cachedVar = osEnv->CallIntMethod(result, getInt);
        }
    }

    class OSMachState {
    public:
        OSMachState(JNIEnv* androidEnv)
            : externalDirectory(androidEnv, statesIds[appStorageDir]),
              cpuExecutor(androidEnv, statesIds[eeExecTechnique]) {}
        void syncAllSettings();
        // Directory with write permissions kSelected by the user
        OSVariable<java::JNIString> externalDirectory;
        OSVariable<java::JNIEnumerator> cpuExecutor;
    };
}
