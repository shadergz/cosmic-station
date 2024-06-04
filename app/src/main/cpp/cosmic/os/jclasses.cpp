#include <os/jclasses.h>
#include <common/global.h>

namespace cosmic::java {
    JniString::JniString(const char* str) {
        utfSide = std::string(str);
        auto kotlinStr{cosmicEnv->NewStringUTF(str)};
        javaRef = cosmicEnv->NewGlobalRef(kotlinStr);
    }

    JniString::JniString(jstring validJniString) {
        auto rawStr{
            cosmicEnv->GetStringUTFChars(validJniString, nullptr)};
        utfSide = std::string(BitCast<const char*>(rawStr));

        cosmicEnv->ReleaseStringUTFChars(validJniString, rawStr);
    }
    JniString::~JniString() {
        if (javaRef)
            cosmicEnv->DeleteGlobalRef(javaRef);
        javaRef = {};
        utfSide = {};
    }
    JniString::JniString(JniString&& str) {
        javaRef = std::exchange(str.javaRef, nullptr);
        utfSide = std::move(str.utfSide);
    }

    JniString::JniString(const std::string str) :
        utfSide(str) {
        javaRef = cosmicEnv->NewGlobalRef(cosmicEnv->NewStringUTF(str.c_str()));
    }
    auto JniString::operator=(const JniString& str) -> JniString& {
        if (javaRef) {
            if (!cosmicEnv->IsSameObject(javaRef, nullptr))
                cosmicEnv->DeleteGlobalRef(javaRef);
        }
        javaRef = cosmicEnv->NewGlobalRef(str.javaRef);
        utfSide = str.utfSide;

        return *this;
    }
    jclass JavaClass::findClass() {
        return cosmicEnv->FindClass(modelName);
    }
}
