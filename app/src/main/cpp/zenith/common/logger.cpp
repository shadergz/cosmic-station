// SPDX-short-identifier: MIT, Version N/A
// This file is protected by the MIT license (please refer to LICENSE.md before making any changes, copying, or redistributing this software)
#include <perfetto.h>

#include <common/logger.h>
PERFETTO_DEFINE_CATEGORIES(
    perfetto::Category("launch")
        .SetDescription("Metrics obtained through the launch of the application"));

// It will expand into a structure containing all possible events, which need to be
// static in the binary sections
PERFETTO_TRACK_EVENT_STATIC_STORAGE();

namespace zenith {
    thread_local fmt::memory_buffer out;

    std::string GlobalLogger::prodPrefix(const LoggerLevel ml) {
        // LEVEL (Thread:Core:ID)
        static thread_local i64 msgCount{};
        i32 coreNum{sched_getcpu()};
        std::array<char, 18> thread;

        pthread_getname_np(pthread_self(), thread.data(), thread.size());
        std::string level{};
        switch (ml) {
        case Verbose:
            level = "Success"; break;
        case Info:
            level = "Info"; break;
        case Error:
            level = "Error"; break;
        default:
            level = "Unk"; break;
        }
        return fmt::format("{} ({}:{}:{}) -> ", level, thread.data(), coreNum, msgCount++);
    }

    GlobalLogger::GlobalLogger() {
#ifndef NDEBUG
        perfetto::TracingInitArgs app;
        app.backends |= perfetto::kSystemBackend;

        perfetto::Tracing::Initialize(app);
        perfetto::TrackEvent::Register();
#endif
    }
}
