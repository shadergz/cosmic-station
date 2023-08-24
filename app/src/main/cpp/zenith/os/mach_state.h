#pragma once

#include <array>
#include <java/classes.h>

namespace zenith::os {
    enum StateId {
        extDirectory,
        coreTechnique

    };
    extern std::array<const std::string, 1> statesIds;
    template <typename T>
    struct OSVariable{
    public:
        OSVariable<T>(const std::string& stateName)
            : m_deviceVar(),
              m_name(stateName)
            {}
        void operator=(const T&& variable) {
            m_deviceVar = variable;
        }
        void fetchValue() {}
        T m_deviceVar;
        std::string m_name;
    };

    class OSMachState {
    public:
        OSMachState() :
            externalDirectory(statesIds[StateId::extDirectory]),
            cpuExecutor(statesIds[StateId::coreTechnique]) {}
        void syncSettings();
        // Directory with write permissions selected by the user
        OSVariable<java::JNIString> externalDirectory;
        OSVariable<java::JNIEnumerator> cpuExecutor;
    };
}
