#pragma once

#include <functional>
#include <jni.h>
#include <common/types.h>
namespace cosmic::gpu {
    class GraphicsLayer;
    class GraphicsFunctionsRef {
    public:
        std::function<void(GraphicsLayer&, jobject)> notifySurfaceChange;
        std::function<void(GraphicsLayer&)> prepareGraphicsApi;
        std::function<void(GraphicsLayer&)> displayApiVersion;
    };
}