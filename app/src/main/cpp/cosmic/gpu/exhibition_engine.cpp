#include <gpu/exhibition_engine.h>
#include <common/except.h>
#include <common/types.h>
namespace cosmic::gpu {
    ExhibitionEngine::ExhibitionEngine(JNIEnv* env) : associated(env) {
    }
    ExhibitionEngine::~ExhibitionEngine() {
        if (globalSurface)
            associated->DeleteGlobalRef(globalSurface);
    }
    void ExhibitionEngine::inheritSurface(jobject surface) {
        if (associated->IsSameObject(surface, nullptr))
            return;
        if (globalSurface)
            associated->DeleteGlobalRef(globalSurface);

        globalSurface = associated->NewGlobalRef(surface);
        if (!globalSurface)
            throw GpuFail("A Surface is required for us to control and inherit to the screen");
        window = ANativeWindow_fromSurface(associated, globalSurface);
        ANativeWindow_acquire(window);
        if (scene.notifySurfaceChange)
            scene.notifySurfaceChange(scene, surface);
        ANativeWindow_release(window);
    }
}