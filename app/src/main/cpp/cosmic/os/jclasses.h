#pragma once

#include <jni.h>
#include <string>

#include <common/types.h>
namespace cosmic::java {
    using JniInteger = jint;
    using JniBool = jboolean;

    class JniString {
    public:
        JniString() {
        }
        JniString(const char* str);
        JniString(const std::string str);
        JniString(jstring validJniString);
        ~JniString();

        auto operator =(const JniString& str) -> JniString&;
        JniString(JniString&& str);
        auto operator *() {
            return utfSide;
        }
        auto operator !=(JniString& differ) {
            return utfSide != differ.utfSide;
        }
        auto get() {
            return utfSide;
        }
        jobject javaRef{};
    private:
        std::string utfSide{""};
    };

    class JavaClass {
    protected:
        JavaClass(const char* className) :
            modelName(className) {}
        virtual ~JavaClass() = default;

        virtual jobject createInstance() = 0;
        virtual void fillInstance(jobject kotlin) = 0;
        virtual void deleteInstance(jobject kotlinBios) = 0;

        jclass findClass();

        const char* modelName;
    };
}
