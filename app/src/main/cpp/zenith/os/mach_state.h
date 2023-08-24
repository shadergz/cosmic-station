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
        OSVariable<T>(std::string stateName) :
            deviceVar(),
            name(stateName) {}
        void operator=(const T&& variable) {
            deviceVar = variable;
        }
        void fetchValue() {
        }
        T deviceVar;
        std::string name;
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
