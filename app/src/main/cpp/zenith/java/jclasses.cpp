#include <java/jclasses.h>

namespace zenith::java {
    JNIString::JNIString(JNIEnv* env, const char* str)
        : validEnv(env) {
        managedStr = std::string(str);
        managedJava = env->NewStringUTF(str);
    }
    JNIString::JNIString(JNIEnv* env, jstring validJniString)
        : validEnv(env) {
        auto rawStr{env->GetStringChars(validJniString, &isCopy)};
        managedStr = std::string(reinterpret_cast<const char*>(rawStr));

        env->ReleaseStringChars(validJniString, rawStr);
    }
    JNIString::~JNIString() {
        if (managedJava)
            validEnv->DeleteLocalRef(static_cast<jobject>(managedJava));
    }
}