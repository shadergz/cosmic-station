#pragma once

#include <array>
#include <java/wr_classes.h>

namespace zenith::os {
    enum StateId {
        extDirectory,
        coreTechnique

    };
    extern std::array<const std::string, 1> statesIds;

    template <typename T>
    struct OSVariable{
    public:
        [[maybe_unused]] OSVariable<T>(std::string stateName) :
            deviceVar(),
            name(stateName) {}
        void operator=(const T&& variable) {
            deviceVar = variable;
        }
        [[maybe_unused]] void fetchValue() {
        }
        [[maybe_unused]] T deviceVar;
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

