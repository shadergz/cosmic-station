#include <gpu/exhibition_engine.h>
#include <common/except.h>
#include <common/types.h>
namespace cosmic::gpu {
    ExhibitionEngine::ExhibitionEngine() {
    }
    void ExhibitionEngine::inheritSurface(JNIEnv* env, jobject surface) {
        if (env->IsSameObject(surface, nullptr))
            return;

        globalSurface = env->NewGlobalRef(surface);
        if (!globalSurface)
            throw GpuFail("A Surface is required for us to control and inherit to the screen");
        window = ANativeWindow_fromSurface(env, globalSurface);
        ANativeWindow_acquire(window);
        if (scene.setSurface)
            scene.setSurface(scene, surface);
        ANativeWindow_release(window);
    }
}