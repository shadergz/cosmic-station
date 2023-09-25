#pragma once

#include <array>

#include <java/classes.h>
#include <impl_types.h>
namespace zenith::os {
    enum StateId {
        extDirectory,
        coreTechnique
    };
    extern std::array<const std::string, 2> statesIds;

    template <typename T>
    struct OSVariable {
    public:
        OSVariable<T>(const std::string& stateName)
            : cachedVar(),
              varName(stateName)
              {}
        void operator=(const T&& variable) {
            cachedVar = variable;
        }
        auto operator*() {
            return cachedVar;
        }
        void updateValue() {}
        T cachedVar;
        std::string varName;
    };

    class OSMachState {
    public:
        OSMachState()
            : m_externalDirectory(statesIds[StateId::extDirectory]),
              m_cpuExecutor(statesIds[StateId::coreTechnique])
              {}
        void synchronizeAllSettings();
        // Directory with write permissions selected by the user
        OSVariable<java::JNIString> m_externalDirectory;
        OSVariable<java::JNIEnumerator> m_cpuExecutor;
    };
}
