#pragma once

#include <jni.h>
#include <android/native_window_jni.h>

#include <gpu/graphics_layer.h>
namespace cosmic::gpu {
    class ExhibitionEngine {
    public:
        ExhibitionEngine();
        ~ExhibitionEngine();
        void inheritSurface(jobject surface);
    private:
        jobject globalSurface{};
        ANativeWindow* window{};

        RenderApi graphics{HardwareOpenGL};
        GraphicsLayer scene{graphics};
    };
}