#pragma once

#include <functional>
#include <jni.h>
#include <common/types.h>
namespace cosmic::gpu {
    class GraphicsLayer;
    class GraphicsFunctionsRef {
    public:
        std::function<void(RawReference<GraphicsLayer>, jobject)> notifySurfaceChange;
        std::function<void(RawReference<GraphicsLayer>)> prepareGraphicsApi;
        std::function<void(RawReference<GraphicsLayer>)> displayApiVersion;
    };
}