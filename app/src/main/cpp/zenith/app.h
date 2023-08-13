#pragma once

#include <memory>

#include <readable_log.h>

namespace zenith {
    class CoreApplication {
    public:
        CoreApplication();
    private:
    };

    [[maybe_unused]] extern std::unique_ptr<CoreApplication> zenithApp;
}
