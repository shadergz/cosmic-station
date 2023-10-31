#include <gpu/exhibition_engine.h>
#include <common/except.h>

namespace zenith::gpu {
    ExhibitionEngine::ExhibitionEngine() {
    }

    void ExhibitionEngine::inheritSurface(JNIEnv* env, jobject surface) {
        if (!env->IsSameObject(surface, nullptr))
            globalSurface = env->NewGlobalRef(surface);

        if (!globalSurface)
            throw GPUFail("A Surface is required for us to control and inherit the presentation screen");

        window = ANativeWindow_fromSurface(env, globalSurface);
    }
}