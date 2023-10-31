#pragma once

#include <android/log.h>
#include <common/types.h>
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
        template <typename T, typename... Args>
        void bind(LoggerLevel msgLevel, const T& format, Args&&... args) {
            fmt::format_to(std::back_inserter(out), fmt::runtime(format), args...);
        }

        template <typename T, typename... Args>
        void success(const T& format, Args&&... args) {
            bind(Info, format, args...);
        }

        [[noreturn]] static void cause(const char* fail) {
            __android_log_assert(fail, tag, "Assertion with a cause, execution flow has been broken");
        }
    private:
        ZenFile logFile{};
        // Don't allow these specific levels to be threaded or printed to the user
        // std::array<u8, 4> refuseLevels{};

        static constexpr auto tag{"Zenith:Backend"};
        fmt::memory_buffer out;
    };
}
