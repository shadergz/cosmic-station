#pragma once

#include <jni.h>
#include <string>

#include <types.h>
namespace zenith::java {
    using JNIEnumerator = jint;

    struct JNIString {
    public:
        JNIString() = default;
        JNIString(JNIEnv* env, const char* str);
        JNIString(JNIEnv* env, const std::string str);
        JNIString(JNIEnv* env, jstring validJniString);

        ~JNIString();

        auto operator *() {
            return managedStr;
        }
        JNIEnv* validEnv;
        std::string managedStr;
        jobject managedJava{};
        jboolean isCopy{};
    };

    class JavaClass {
    protected:
        JavaClass(JNIEnv* env, const char* className)
            : classEnv(env),
              model(env->FindClass(className)) {}
        virtual ~JavaClass() = default;

        virtual jobject createInstance() = 0;
        virtual void fillInstance(jobject kotlin) = 0;

        JNIEnv* classEnv{};
        jclass model{};
    };
}
