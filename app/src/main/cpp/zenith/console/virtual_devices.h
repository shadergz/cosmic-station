#pragma once

#include <memory>

#include <eeiv/ee_engine.h>
#include <console/global_memory.h>

namespace console {
    class VirtualDevices {
    public:
        VirtualDevices(const std::shared_ptr<GlobalMemory>& blocks);
        std::shared_ptr<eeiv::EmotionMIPS> mipsEER5900;

    };

}

