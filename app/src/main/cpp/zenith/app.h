#pragma once

#include <memory>

#include <eeiv/ee_engine.h>
#include <paper_log.h>

namespace zenith {
    class CoreApplication {
    public:
        CoreApplication();
    private:
        std::shared_ptr<eeiv::EECoreCpu> coreMips;
    };

    extern std::unique_ptr<CoreApplication> zenithApp;
}
