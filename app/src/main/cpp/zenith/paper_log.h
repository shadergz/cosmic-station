#pragma once

#include <android/log.h>
#include <vector>

#include <basic_types.h>

namespace zenith {
    enum PaleLevel {
        PaleInfo [[maybe_unused]]    = ANDROID_LOG_INFO,

        PaleDebug [[maybe_unused]]   = ANDROID_LOG_DEBUG,
        PaleVerbose [[maybe_unused]] = ANDROID_LOG_VERBOSE,

        PaleError [[maybe_unused]]   = ANDROID_LOG_ERROR,
    };

    class PalePaper {
    public:
    private:
        [[maybe_unused]] ZenFile logFile{};
        // Don't allow these specific levels to be threaded or printed to the user
        [[maybe_unused]] std::vector<PaleLevel> refuseLevels{};
    };

    [[maybe_unused]] extern std::shared_ptr<PalePaper> userLog;
}
