#pragma once

#include <jni.h>
#include <android/native_window_jni.h>

#include <gpu/graphics_layer.h>
namespace cosmic::gpu {
    class ExhibitionEngine {
    public:
        ExhibitionEngine(JNIEnv* env);
        ~ExhibitionEngine();
        void inheritSurface(jobject surface);
    private:
        jobject globalSurface{};
        JNIEnv* associated{};
        ANativeWindow* window{};

        RenderApi graphics{HardwareOpenGL};
        GraphicsLayer scene{graphics};
    };
}