#pragma once

#include <memory>

#include <link/main_memory.h>
#include <eeiv/ee_engine.h>
#include <iop/iop_core.h>
namespace zenith::console {
    class VirtDevices {
    public:
        VirtDevices(std::shared_ptr<link::GlobalMemory>& blocks);
        std::shared_ptr<eeiv::EEMipsCore> mipsEER5900;
        std::shared_ptr<iop::IOMipsCore> mipsIOP;

    };

}

