#pragma once

#include <jni.h>
#include <string>

#include <types.h>
namespace zenith::java {
    using JNIEnumerator = i32;
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
}
