#pragma once

#include <jni.h>
#include <android/native_window_jni.h>

#include <gpu/violet/graphics_layer.h>
namespace cosmic::gpu {
    class ExhibitionEngine {
    public:
        ExhibitionEngine();
        void inheritSurface(JNIEnv* env, jobject surface);
    private:
        jobject globalSurface;
        ANativeWindow* window;

        violet::RenderApi graphics{violet::HardwareOpenGL};
        violet::VioletLayer scene{graphics};
    };
}