#include <profiler/rec_tracer.h>

PERFETTO_DEFINE_CATEGORIES(
    perfetto::Category("launch")
        .SetDescription("Metrics obtained through the launch of the application"));

// It will expand into a structure containing all possible events, which need to be
// static in the binary sections
PERFETTO_TRACK_EVENT_STATIC_STORAGE();

namespace addons::profiler {
    void Tracing::startTracer() {
        // Write all records to the `traced` daemon, to improve speed and accuracy during tracing
        app.backends |= perfetto::kSystemBackend;

        perfetto::Tracing::Initialize(app);
        perfetto::TrackEvent::Register();
    }
}
