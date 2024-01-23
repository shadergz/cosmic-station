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
        template <typename... Args>
        void bind(LoggerLevel msgLevel, fmt::format_string<Args...>& format, Args&&... args) {
            for (auto deny : disableLevels) {
                if (deny == msgLevel) {
                    return;
                }
            }

            fmt::format_to(std::back_inserter(out), "{}", prodPrefix(msgLevel));
            fmt::format_to(std::back_inserter(out), fmt::runtime(format), std::forward<Args>(args)...);
            fmt::format_to(std::back_inserter(out), "\n");

            __android_log_write(static_cast<android_LogPriority>(msgLevel), tag, out.data());
            out.clear();
        }

        template <typename... Args>
        void success(fmt::format_string<Args...> format, Args&&... args) {
            bind(Verbose, format, std::forward<Args>(args)...);
        }
        template <typename... Args>
        void info(fmt::format_string<Args...> format, Args&&... args) {
            bind(Info, format, std::forward<Args>(args)...);
        }
        template <typename... Args>
        void debug(fmt::format_string<Args...> format, Args&&... args) {
            bind(Debug, format, std::forward<Args>(args)...);
        }
        template <typename... Args>
        void error(fmt::format_string<Args...> format, Args&&... args) {
            bind(Error, format, std::forward<Args>(args)...);
        }
        [[noreturn]] static void cause(const char* fail) {
            __android_log_assert(fail, tag, "Assertion with a cause, execution flow has been broken");
        }
    private:
        std::string prodPrefix(const LoggerLevel ml);
        DescriptorRaii logFile{};
        // Don't allow these specific levels to be threaded or printed to the user
        std::array<u8, 4> disableLevels{};

        static constexpr auto tag{"Cosmic"};
    };
}
