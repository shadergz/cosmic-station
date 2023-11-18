#pragma once

#include <functional>
#include <jni.h>

namespace cosmic::gpu::violet {
    class VioletPayload {
    public:
        std::function<void(jobject)> glvkSetSurface;
    };
}