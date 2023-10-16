#pragma once

#include <perfetto.h>

namespace addons::profiler {
    class Tracing {
    public:
        void startTracer();
    private:
        perfetto::TracingInitArgs app;
    };
}
