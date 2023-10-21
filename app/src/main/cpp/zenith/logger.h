#pragma once

#include <android/log.h>
#include <array>

#include <types.h>

namespace zenith {
enum LoggerLevel {
    Info = ANDROID_LOG_INFO,
    Debug = ANDROID_LOG_DEBUG,
    Verbose = ANDROID_LOG_VERBOSE,
    Error = ANDROID_LOG_ERROR,
};

class GlobalLogger {
public:
    GlobalLogger();
    [[noreturn]] static void cause(const char* fail) {
        __android_log_assert(fail, tag, "Assertion with a cause, execution flow has been broken");
    }
private:
    ZenFile logFile{};
    // Don't allow these specific levels to be threaded or printed to the user
    // std::array<u8, 4> refuseLevels{};

    static constexpr auto tag{"Zenith:Backend"};
};
}
