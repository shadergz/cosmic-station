#pragma once

#include <exception>
#include <string>
#include <fmt/format.h>

#include <jni.h>
namespace cosmic {
    class CosmicException : public std::runtime_error {
    protected:
        CosmicException(const std::string& format);

    public:
        static void setExceptionClass(jobject super);
    private:
        jstring lookupByActivity();
        void alertUser();

        jstring msg{};
        jstring title{};
        jmethodID alert{};
    };

#define DECLARE_EXCEPTION_TYPE(name, tag)\
    class name : public CosmicException {\
    public:\
        template <typename... Args>\
        name(fmt::format_string<Args...> format, Args&&... args) :\
            CosmicException("(" tag ") " + fmt::format(format, std::forward<Args>(args)...)) {}\
    }

    DECLARE_EXCEPTION_TYPE(Cop0Err, "Cop0");
    DECLARE_EXCEPTION_TYPE(TimerErr, "EE::Timer");
    DECLARE_EXCEPTION_TYPE(MioErr, "MIO");
    DECLARE_EXCEPTION_TYPE(IoErr, "IO");
    DECLARE_EXCEPTION_TYPE(FsErr, "FS");
    DECLARE_EXCEPTION_TYPE(GpuErr, "GPU");
    DECLARE_EXCEPTION_TYPE(AppErr, "Cosmic");
#undef DECLARE_EXCEPTION_TYPE
}
