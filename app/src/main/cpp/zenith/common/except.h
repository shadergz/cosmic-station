#pragma once

#include <exception>
#include <string>
#include <fmt/format.h>
namespace zenith {
    class FatalError : public std::runtime_error {
    protected:
        template <typename T, typename... Args>
        FatalError(const T& format, Args&&... args)
            : std::runtime_error(fmt::format(fmt::runtime(format), args...)) {}
        template <typename T, typename... Args>
        FatalError(const T& format)
            : std::runtime_error(format) {}
    };

#define DeclareExceptionType(name, tag)\
    class name : FatalError {\
    public:\
        template <typename T, typename... Args>\
        name(T& format, Args&&... args) : FatalError("(" tag ") " + std::string(format), args...) {}\
        template <typename T, typename... Args>\
        name(T& format) : FatalError("(" tag ") " + std::string(format)) {}\
    }

    DeclareExceptionType(Cop0Fail, "Cop0");
    DeclareExceptionType(TimerFail, "Timer");
    DeclareExceptionType(MMUFail, "MMU");
    DeclareExceptionType(IOFail, "IO");
    DeclareExceptionType(FSFail, "FS");
    DeclareExceptionType(GPUFail, "GPU");
    DeclareExceptionType(AppFail, "Zenith");

#undef DeclareExceptionType

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
