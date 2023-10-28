#pragma once

#include <array>
#include <cassert>

#include <java/jclasses.h>
#include <types.h>
namespace zenith::os {
    enum StateIDs : u16 {
        AppStorage,
        GpuTurboMode,
        GpuCustomDriver,
        eeMode
    };
    extern std::array<const std::string, 4> dsKeys;

    template <typename T>
    struct OSVariable {
    public:
        OSVariable<T>(JNIEnv* androidEnv, const std::string& stateName)
            : osEnv(androidEnv), cachedVar() {
            auto state{osEnv->NewStringUTF(stateName.data())};
            varName = static_cast<jstring>(osEnv->NewGlobalRef(state));

            osEnv->DeleteLocalRef(state);
        }
        ~OSVariable() {
            osEnv->DeleteGlobalRef(varName);
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
            "getDataStoreValue", "(Ljava/lang/String;)Ljava/lang/Object;")};
        if (osEnv->ExceptionCheck())
            osEnv->ExceptionOccurred();
        auto result{osEnv->CallStaticObjectMethod(settingsClass, updateEnvMethod, varName)};

        if constexpr (std::is_same<T, java::JNIString>::value) {
            cachedVar = java::JNIString(osEnv, bit_cast<jstring>(result));
        } else if constexpr (std::is_same<T, java::JNIInteger>::value) {
            auto getInt{osEnv->GetMethodID(osEnv->GetObjectClass(result), "intValue", "()I")};
            cachedVar = osEnv->CallIntMethod(result, getInt);
        } else if constexpr (std::is_same<T, java::JNIBool>::value) {
            assert(osEnv->IsInstanceOf(result, osEnv->FindClass("java/lang/Boolean")));
            auto getBool{osEnv->GetMethodID(osEnv->GetObjectClass(result), "booleanValue", "()Z")};
            cachedVar = osEnv->CallBooleanMethod(result, getBool);
        }

        osEnv->DeleteLocalRef(result);
    }

    class OSMachState {
    public:
        OSMachState(JNIEnv* androidEnv) :
            appStorage(androidEnv, dsKeys.at(AppStorage)),
            turboMode(androidEnv, dsKeys.at(GpuTurboMode)),
            customDriver(androidEnv, dsKeys.at(GpuCustomDriver)),
            eeModeWay(androidEnv, dsKeys.at(eeMode)) {

        }
        void syncAllSettings();

        // Directory with write permissions kSelected by the user
        OSVariable<java::JNIString> appStorage;
        OSVariable<java::JNIBool> turboMode;
        OSVariable<java::JNIString> customDriver;
        OSVariable<java::JNIInteger> eeModeWay;
    };
}
