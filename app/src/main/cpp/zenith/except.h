#pragma once

#include <exception>
#include <string>
namespace zenith {
    class FatalError : public std::runtime_error {
    protected:
        template <typename T, typename... Args>
        FatalError(const T& format, Args&&... args)
            : std::runtime_error(fmt::format(fmt::runtime(format), args...)) {}
    };

#define DeclareExceptionType(name, tag) \
    class name : FatalError {\
    public:\
    template <typename T, typename... Args>\
        name(T& format, Args&&... args) : FatalError("(" tag ") " + std::string(format), args...) {}\
    }

    DeclareExceptionType(Cop0Fail, "Cop0");
    DeclareExceptionType(MMUFail, "MMU");
    DeclareExceptionType(IOFail, "IO");
    DeclareExceptionType(FSFail, "FS");
    DeclareExceptionType(AppFail, "Zenith");

#undef DeclareExceptionType
}
