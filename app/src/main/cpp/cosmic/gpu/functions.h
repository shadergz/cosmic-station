#pragma once

#include <functional>
#include <jni.h>
#include <common/types.h>
namespace cosmic::gpu {
    class GraphicsLayer;
    class GraphicsFunctionsRef {
    public:
        std::function<void(raw_reference<GraphicsLayer>, jobject)> notifySurfaceChange;
        std::function<void(raw_reference<GraphicsLayer>)> prepareGraphicsApi;
        std::function<void(raw_reference<GraphicsLayer>)> displayApiVersion;
    };
}