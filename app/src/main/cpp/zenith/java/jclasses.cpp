#include <java/jclasses.h>

namespace zenith::java {
    JNIString::JNIString(JNIEnv* env, const char* str)
        : validEnv(env) {
        readableStr = std::string(str);
        javaRef = env->NewStringUTF(str);
    }

    JNIString::JNIString(JNIEnv* env, jstring validJniString)
        : validEnv(env) {
        auto rawStr{env->GetStringUTFChars(validJniString, nullptr)};
        readableStr = std::string(bit_cast<const char*>(rawStr));

        env->ReleaseStringUTFChars(validJniString, rawStr);
    }
    JNIString::~JNIString() {
        if (javaRef)
            validEnv->DeleteLocalRef(javaRef);
    }

    JNIString::JNIString(JNIEnv* env, const std::string str)
        : validEnv(env), readableStr(str) {
        javaRef = env->NewStringUTF(str.c_str());
    }
}
