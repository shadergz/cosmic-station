#include <java/jclasses.h>

namespace zenith::java {
    JNIString::JNIString(JNIEnv* env, const char* str)
        : validEnv(env) {
        readableStr = std::string(str);
        auto kotlinStr{env->NewStringUTF(str)};
        javaRef = env->NewGlobalRef(kotlinStr);
    }

    JNIString::JNIString(JNIEnv* env, jstring validJniString)
        : validEnv(env) {
        auto rawStr{env->GetStringUTFChars(validJniString, nullptr)};
        readableStr = std::string(bit_cast<const char*>(rawStr));

        env->ReleaseStringUTFChars(validJniString, rawStr);
    }
    JNIString::~JNIString() {
        if (javaRef)
            validEnv->DeleteGlobalRef(javaRef);
    }

    JNIString::JNIString(JNIEnv* env, const std::string str)
        : validEnv(env), readableStr(str) {
        javaRef = env->NewGlobalRef(env->NewStringUTF(str.c_str()));
    }
}
