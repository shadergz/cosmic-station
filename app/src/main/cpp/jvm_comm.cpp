#include <jni.h>
#include <signal.h>

#include <cosmic/common/global.h>
#include <cosmic/os/jclasses.h>
static std::array<struct sigaction, 3> signals;
void catchSystemSignals(cosmic::i32 sig, siginfo_t* ino, void* context) {
    cosmic::u8 sid{};
    if (sig == SIGABRT || sig == SIGTRAP) {
        if (sig == SIGTRAP)
            sid = 1;
    }
    if (sig == SIGSEGV) {
        sid = 2;
    }
    sigaction(sig, &signals[sid], nullptr);
}
static struct sigaction trap{
    .sa_flags = SA_SIGINFO,
    .sa_sigaction = catchSystemSignals,
};

// JNI_OnLoad function is called when the JVM has loaded our native code in the heap, this process
// is started by Java Runtime using System.loadLibrary("cosmic")
extern "C" jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    void* env{};
    if (vm)
        vm->GetEnv(&env, JNI_VERSION_1_6);
    cosmic::cosmicEnv.feedVm(cosmic::BitCast<JNIEnv*>(env));

    // Kickstart the user readable log system also called as, GlobalLogger
    cosmic::user = std::make_shared<cosmic::GlobalLogger>();
    cosmic::states = std::make_unique<cosmic::os::OsMachState>();

    sigaction(SIGABRT, &trap, &signals[0]);
    sigaction(SIGTRAP, &trap, &signals[1]);
    sigaction(SIGSEGV, &trap, &signals[2]);

    cosmic::app = std::make_shared<cosmic::CoreApplication>();
    return JNI_VERSION_1_6;
}
extern "C"
JNIEXPORT void JNICALL
Java_emu_cosmic_MainActivity_syncSettings(JNIEnv* env, jobject thiz, jstring dateTime) {
    cosmic::app->lastSetSync = cosmic::java::JniString(dateTime).get();
    cosmic::states->syncAllSettings();

    cosmic::user->success("Time of the last synchronization of global settings: {}", cosmic::app->lastSetSync);
}
