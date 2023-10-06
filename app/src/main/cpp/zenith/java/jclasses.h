#pragma once

#include <jni.h>
#include <string>

#include <types.h>
namespace zenith::java {
    using JNIEnumerator = jint;

    struct JNIString {
    public:
        JNIString() : managedStr(), isCopy() {}
        JNIString(JNIEnv* env, jstring validJniString)
            : isCopy() {
            auto rawStr{env->GetStringChars(validJniString, &isCopy)};
            managedStr = std::string(reinterpret_cast<const char*>(rawStr));

            env->ReleaseStringChars(validJniString, rawStr);
        }
        auto operator *() {
            return managedStr;
        }

        std::string managedStr;
        jboolean isCopy;
    };

    class JavaClass {
    protected:
        JavaClass(JNIEnv* env, const char* className)
            : classEnv(env),
              model(env->FindClass(className)) {}
        JNIEnv* classEnv{};
        jclass model{};

        virtual jobject createInstance() = 0;
        virtual void fillInstance(jobject kotlin) = 0;
    };
}
