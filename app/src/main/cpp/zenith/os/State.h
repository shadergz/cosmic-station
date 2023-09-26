#pragma once

#include <array>

#include <java/Classes.h>
#include <ImplTypes.h>
namespace zenith::os {
    enum StateIDs {
        extDirectory,
        coreTechnique
    };
    extern std::array<const std::string, 2> statesIds;

    template <typename T>
    struct OSVariable {
    public:
        OSVariable<T>(const std::string& stateName)
            : cachedVar(),
              varName(stateName) {}
        void operator=(const T&& variable) {
            cachedVar = variable;
        }
        auto operator*() {
            return cachedVar;
        }
        void updateValue();
        T cachedVar;
        std::string varName;
    };

    template<typename T>
    void OSVariable<T>::updateValue() {

    }

    class OSMachState {
    public:
        OSMachState()
            : externalDirectory(statesIds[StateIDs::extDirectory]),
              cpuExecutor(statesIds[StateIDs::coreTechnique]) {}
        void synchronizeAllSettings();
        // Directory with write permissions selected by the user
        OSVariable<java::JNIString> externalDirectory;
        OSVariable<java::JNIEnumerator> cpuExecutor;
    };
}
