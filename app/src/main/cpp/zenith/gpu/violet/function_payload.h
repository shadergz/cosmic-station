#pragma once

#include <functional>
#include <jni.h>

namespace zenith::gpu::violet {
    class VioletPayload {
    public:
        std::function<void(jobject)> glvkSetSurface;
    };
}