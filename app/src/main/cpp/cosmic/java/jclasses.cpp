#include <java/jclasses.h>

namespace cosmic::java {
    JniString::JniString(JNIEnv* env, const char* str) :
        validEnv(env) {
        readableStr = std::string(str);
        auto kotlinStr{env->NewStringUTF(str)};
        javaRef = validEnv->NewGlobalRef(kotlinStr);
    }

    JniString::JniString(JNIEnv* env, jstring validJniString) :
        validEnv(env) {
        auto rawStr{env->GetStringUTFChars(validJniString, nullptr)};
        readableStr = std::string(BitCast<const char *>(rawStr));

        env->ReleaseStringUTFChars(validJniString, rawStr);
    }
    JniString::~JniString() {
        if (javaRef)
            validEnv->DeleteGlobalRef(javaRef);
    }

    JniString::JniString(JNIEnv* env, const std::string str) :
        validEnv(env), readableStr(str) {
        javaRef = env->NewGlobalRef(env->NewStringUTF(str.c_str()));
    }
}
