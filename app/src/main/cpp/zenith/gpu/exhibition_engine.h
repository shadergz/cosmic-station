#pragma once

#include <jni.h>
#include <android/native_window_jni.h>

#include <gpu/violet/layer.h>
namespace zenith::gpu {
    class ExhibitionEngine {
    public:
        ExhibitionEngine();

        void inheritSurface(JNIEnv* env, jobject surface);

    private:
        jobject globalSurface;
        ANativeWindow* window;

        violet::VioletLayer scene;
    };
}