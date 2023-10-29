#include <perfetto.h>

#include <common/logger.h>

PERFETTO_DEFINE_CATEGORIES(
    perfetto::Category("launch")
        .SetDescription("Metrics obtained through the launch of the application"));

// It will expand into a structure containing all possible events, which need to be
// static in the binary sections
PERFETTO_TRACK_EVENT_STATIC_STORAGE();

namespace zenith {
    GlobalLogger::GlobalLogger() {
        out = fmt::memory_buffer();
#ifndef NDEBUG
        perfetto::TracingInitArgs app;
        app.backends |= perfetto::kSystemBackend;

        perfetto::Tracing::Initialize(app);
        perfetto::TrackEvent::Register();
#endif
    }
}
