#pragma once

#include <android/log.h>
#include <vector>

#include <impl_types.h>

namespace zenith {
    enum PaleLevel {
        PaleInfo = ANDROID_LOG_INFO,

        PaleDebug = ANDROID_LOG_DEBUG,
        PaleVerbose = ANDROID_LOG_VERBOSE,

        PaleError = ANDROID_LOG_ERROR,
    };

    class PalePaper {
    public:
    private:
        ZenFile logFile{};
        // Don't allow these specific levels to be threaded or printed to the user
        std::vector<PaleLevel> refuseLevels{};
    };

    extern std::shared_ptr<PalePaper> userLog;
}
