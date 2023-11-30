#include <jni.h>
#include <cosmic/common/global.h>

extern "C"
JNIEXPORT void JNICALL
Java_emu_cosmic_EmulationActivity_swtSurfaceContext(JNIEnv* env, jobject thiz, jobject surface) {
    cosmic::app->scene->inheritSurface(env, surface);
}
extern "C"
JNIEXPORT void JNICALL
Java_emu_cosmic_EmulationActivity_runEmuVm(JNIEnv *env, jobject thiz) {
    cosmic::app->vm->startVM();
}