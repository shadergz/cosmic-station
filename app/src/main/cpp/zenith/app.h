#pragma once

#include <memory>

namespace zenith {
    class CoreApplication {
    public:
        CoreApplication();
    private:
    };

    [[maybe_unused]] extern std::unique_ptr<CoreApplication> zenithApp;
}
