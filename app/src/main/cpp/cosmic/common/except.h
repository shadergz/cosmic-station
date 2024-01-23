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

    DECLARE_EXCEPTION_TYPE(Cop0Fail, "Cop0");
    DECLARE_EXCEPTION_TYPE(TimerFail, "EE.Timer");
    DECLARE_EXCEPTION_TYPE(MioFail, "MIO");
    DECLARE_EXCEPTION_TYPE(IoFail, "IO");
    DECLARE_EXCEPTION_TYPE(FsFail, "FS");
    DECLARE_EXCEPTION_TYPE(GpuFail, "GPU");
    DECLARE_EXCEPTION_TYPE(AppFail, "Cosmic");
#undef DECLARE_EXCEPTION_TYPE

    class NonAbort : public std::bad_exception {
    public:
        template <typename T, typename... Args>
        NonAbort(const T& format, Args&&... args) {
            fmt::format_to(std::back_inserter(cause), fmt::runtime(format), args...);
        }
        template <typename T>
        NonAbort(const T& format) {
            fmt::format_to(std::back_inserter(cause), fmt::runtime(format));
        }
        const char* what() const noexcept {
            return cause.data();
        }
    private:
        fmt::memory_buffer cause;
    };
}
