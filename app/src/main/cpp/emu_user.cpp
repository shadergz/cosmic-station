#include <thread>
#include <jni.h>
#include <cosmic/common/global.h>

extern "C"
JNIEXPORT void JNICALL
Java_emu_cosmic_EmulationActivity_swtSurfaceContext(JNIEnv* env, jobject thiz, jobject surface) {
    cosmic::app->scene->inheritSurface(surface);
}

std::atomic<cosmic::u8> is{false};
extern "C"
JNIEXPORT void JNICALL
Java_emu_cosmic_EmulationActivity_startEmulator(JNIEnv* env, [[maybe_unused]] jobject thiz) {
    cosmic::CosmicException::setExceptionClass(thiz);

    cosmic::app->vm->resetVm();
    is = 1;
    cosmic::app->vm->startVm();

    for (; is ;)
        std::this_thread::sleep_for(std::chrono::nanoseconds(10'000));
    cosmic::app->vm->stopVm();
}
extern "C"
JNIEXPORT void JNICALL
Java_emu_cosmic_EmulationActivity_stopEmulator(JNIEnv* env, jobject thiz) {
    if (is != 0)
        is = 0;
}