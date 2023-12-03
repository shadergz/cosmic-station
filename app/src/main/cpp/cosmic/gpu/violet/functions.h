#pragma once

#include <functional>
#include <jni.h>
#include <common/types.h>
namespace cosmic::gpu::violet {
    class VioletLayer;
    class VioletFunctionsRef {
    public:
        std::function<void(raw_reference<VioletLayer>, jobject)> setSurface;
        std::function<void(raw_reference<VioletLayer>)> prepareGraphicsApi;
        std::function<void(raw_reference<VioletLayer>)> displayApiVersion;
    };
}