// Adding our events to Perfetto's system
#include "perfetto/sdk/perfetto.h"

PERFETTO_DEFINE_CATEGORIES(
    perfetto::Category("launch")
        .SetDescription("Metrics obtained through the launch of the application"));

// It will expand into a structure containing all possible events, which need to be
// static in the binary sections
PERFETTO_TRACK_EVENT_STATIC_STORAGE();

namespace addons {
    void enbPerfettoProfiler(perfetto::TracingInitArgs& appArgs) {
        perfetto::Tracing::Initialize(appArgs);
        perfetto::TrackEvent::Register();
    }
}

