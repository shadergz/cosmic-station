#pragma once

#include <functional>
#include <jni.h>
namespace cosmic::gpu::violet {
    class VioletLayer;
    class VioletPayload {
    public:
        std::function<void(VioletLayer&, jobject)> glvkSetSurface;
        std::function<void(VioletLayer&)> establishGraphicsAPI;
    };
}