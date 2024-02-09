#include <gpu/exhibition_engine.h>
#include <common/except.h>
#include <common/types.h>
#include <common/global.h>
namespace cosmic::gpu {
    ExhibitionEngine::ExhibitionEngine() {
    }
    ExhibitionEngine::~ExhibitionEngine() {
        if (globalSurface)
            cosmicEnv->DeleteGlobalRef(globalSurface);
    }
    void ExhibitionEngine::inheritSurface(jobject surface) {
        if (cosmicEnv->IsSameObject(surface, nullptr))
            return;
        if (globalSurface)
            cosmicEnv->DeleteGlobalRef(globalSurface);

        globalSurface = cosmicEnv->NewGlobalRef(surface);
        if (!globalSurface) {
            throw GpuErr("A Surface is required for us to control and inherit to the screen");
        }
        window = ANativeWindow_fromSurface(*cosmicEnv, globalSurface);
        ANativeWindow_acquire(window);
        if (scene.notifySurfaceChange)
            scene.notifySurfaceChange(scene, surface);
        ANativeWindow_release(window);
    }
}