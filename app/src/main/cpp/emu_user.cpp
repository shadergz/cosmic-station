#include <thread>
#include <jni.h>
#include <cosmic/common/global.h>

extern "C"
JNIEXPORT void JNICALL
Java_emu_cosmic_EmulationActivity_swtSurfaceContext(JNIEnv* env, jobject thiz, jobject surface) {
    cosmic::app->scene->inheritSurface(env, surface);
}

std::atomic<cosmic::u8> is{false};
extern "C"
JNIEXPORT void JNICALL
Java_emu_cosmic_EmulationActivity_runEmulatorVm(JNIEnv* env, jobject thiz) {
    cosmic::app->vm->resetVM();
    cosmic::app->vm->startVM();
    is = 1;

    for (; is ;)
        std::this_thread::sleep_for(std::chrono::nanoseconds(10'000));
    cosmic::app->vm->stopVM();
}
extern "C"
JNIEXPORT void JNICALL
Java_emu_cosmic_EmulationActivity_stopEmulatorVm(JNIEnv* env, jobject thiz) {
    is = 0;
}