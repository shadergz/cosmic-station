#pragma once

#include <functional>
#include <jni.h>
#include <common/types.h>
namespace cosmic::gpu::violet {
    class VioletLayer;
    class VioletFunctionsReferences {
    public:
        std::function<void(raw_reference<VioletLayer>, jobject)> glvkSetSurface;
        std::function<void(raw_reference<VioletLayer>)> prepareGraphicsApi;
        std::function<void(raw_reference<VioletLayer>)> displayApiVersion;
    };
}