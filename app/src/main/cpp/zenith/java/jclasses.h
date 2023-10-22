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

        JNIString& operator=(JNIString&& str) noexcept {
            str.validEnv = validEnv;
            javaRef = str.javaRef;
            readableStr = str.readableStr;

            str.javaRef = nullptr;
            return *this;
        }
        JNIString(JNIString&& str) {
            *this = std::move(str);
        }
        ~JNIString();
        auto operator *() {
            return readableStr;
        }
        JNIEnv* validEnv;
        std::string readableStr;
        jobject javaRef{};
        jboolean isCopy{};
    };

    class JavaClass {
    protected:
        JavaClass(JNIEnv* env, const char* className)
            : classEnv(env),
              modelName(className) {}
        virtual ~JavaClass() = default;

        virtual jobject createInstance() = 0;
        virtual void fillInstance(jobject kotlin) = 0;
        jclass findClass() {
            return classEnv->FindClass(modelName);
        }

        JNIEnv* classEnv{};
        const char* modelName;
    };
}
