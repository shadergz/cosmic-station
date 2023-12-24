#pragma once

#include <jni.h>
#include <string>

#include <common/types.h>
namespace cosmic::java {
    using JniInteger = jint;
    using JniBool = jboolean;

    struct JniString {
    public:
        JniString() = default;
        JniString(JNIEnv* env, const char* str);
        JniString(JNIEnv* env, const std::string str);
        JniString(JNIEnv* env, jstring validJniString);
        JniString(JniString&& str) {
            *this = std::move(str);
        }
        JniString(JniString& str) {
            validEnv = str.validEnv;
            javaRef = validEnv->NewGlobalRef(str.javaRef);
            readableStr = str.readableStr;
        }
        ~JniString();

        JniString& operator=(JniString&& str) noexcept {
            validEnv = str.validEnv;
            if (javaRef) {
                if (!validEnv->IsSameObject(javaRef, nullptr))
                    validEnv->DeleteGlobalRef(javaRef);
            }
            javaRef = std::exchange(str.javaRef, nullptr);
            readableStr = str.readableStr;

            return *this;
        }
        auto operator *() {
            return readableStr;
        }
        auto operator !=(JniString& differ) {
            return readableStr != differ.readableStr;
        }

        JNIEnv* validEnv{};
        std::string readableStr{""};
        jobject javaRef{};
        jboolean isCopy{false};
    };

    class JavaClass {
    protected:
        JavaClass(JNIEnv* env, const char* className) :
            classEnv(env),
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
