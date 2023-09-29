#include <logger.h>

namespace zenith {
    void verifyRtCheck(bool condition, std::function<void()> func) {
        [[unlikely]] if (condition)
            func();
    }
}
