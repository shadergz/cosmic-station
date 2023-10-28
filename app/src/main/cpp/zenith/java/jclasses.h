#pragma once

#include <jni.h>
#include <string>

#include <types.h>
namespace zenith::java {
    using JNIInteger = jint;
    using JNIBool = jboolean;

    struct JNIString {
    public:
        JNIString() = default;
        JNIString(JNIEnv* env, const char* str);
        JNIString(JNIEnv* env, const std::string str);
        JNIString(JNIEnv* env, jstring validJniString);
        JNIString(JNIString&& str) {
            *this = std::move(str);
        }
        JNIString(JNIString& str) {
            validEnv = str.validEnv;
            javaRef = validEnv->NewGlobalRef(str.javaRef);
            readableStr = str.readableStr;
        }
        ~JNIString();

        JNIString& operator=(JNIString&& str) noexcept {
            if (javaRef) {
                if (!validEnv->IsSameObject(javaRef, nullptr))
                    validEnv->DeleteGlobalRef(javaRef);
            }
            validEnv = str.validEnv;
            javaRef = std::exchange(str.javaRef, nullptr);
            readableStr = str.readableStr;

            return *this;
        }
        auto operator *() {
            return readableStr;
        }
        JNIEnv* validEnv{};
        std::string readableStr{""};
        jobject javaRef{};
        jboolean isCopy{false};
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
