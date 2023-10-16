#include <logger.h>
#include <perfetto.h>

namespace addons {
    extern void enbPerfettoProfiler(perfetto::TracingInitArgs& appArgs);
}

namespace zenith {
    void verifyRtCheck(bool condition, std::function<void()> func) {
        [[unlikely]] if (condition)
            func();
    }

    GlobalLogger::GlobalLogger() {
        perfetto::TracingInitArgs args;
        // Write all records to the `traced` daemon, to improve speed and accuracy during tracing
        args.backends |= perfetto::kSystemBackend;

        addons::enbPerfettoProfiler(args);
    }
}
