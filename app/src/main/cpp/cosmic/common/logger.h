// SPDX-short-identifier: MIT, Version N/A
// This file is protected by the MIT license (please refer to LICENSE.md before making any changes, copying, or redistributing this software)
#pragma once

#include <android/log.h>
#include <common/types.h>
namespace cosmic {
    enum LoggerLevel {
        Info = ANDROID_LOG_INFO,
        Debug = ANDROID_LOG_DEBUG,
        Verbose = ANDROID_LOG_VERBOSE,
        Error = ANDROID_LOG_ERROR,
    };

    extern thread_local fmt::memory_buffer out;

    class GlobalLogger {
    public:
        GlobalLogger();
        template <typename T, typename... Args>
        void bind(LoggerLevel msgLevel, const T& format, Args&&... args) {
            for (auto deny : disableLevels) {
                if (deny == msgLevel)
                    return;
            }

            fmt::format_to(std::back_inserter(out), "{}", prodPrefix(msgLevel));
            fmt::format_to(std::back_inserter(out), fmt::runtime(format), args...);
            fmt::format_to(std::back_inserter(out), "\n");

            __android_log_write(static_cast<android_LogPriority>(msgLevel), tag, out.data());
            out.clear();
        }

        template <typename T, typename... Args>
        void success(const T& format, Args&&... args) {
            bind(Verbose, format, args...);
        }
        template <typename T, typename... Args>
        void info(const T& format, Args&&... args) {
            bind(Info, format, args...);
        }
        template <typename T, typename... Args>
        void debug(const T& format, Args&&... args) {
            bind(Debug, format, args...);
        }
        template <typename T, typename... Args>
        void error(const T& format, Args&&... args) {
            bind(Error, format, args...);
        }
        [[noreturn]] static void cause(const char* fail) {
            __android_log_assert(fail, tag, "Assertion with a cause, execution flow has been broken");
        }
    private:
        std::string prodPrefix(const LoggerLevel ml);
        ZenFile logFile{};
        // Don't allow these specific levels to be threaded or printed to the user
        std::array<u8, 4> disableLevels{};

        static constexpr auto tag{"Cosmic"};
    };
}
