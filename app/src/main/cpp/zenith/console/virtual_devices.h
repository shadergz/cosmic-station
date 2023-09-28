#pragma once

#include <memory>

#include <link/managed_glb_memory.h>
#include <eeiv/ee_engine.h>
#include <iop/iop_core.h>
namespace zenith::console {
    class VirtualDevices {
    public:
        VirtualDevices(const std::shared_ptr<GlobalMemory>& blocks);
        std::shared_ptr<eeiv::EEMipsCore> mipsEER5900;
        std::shared_ptr<iop::IOMipsCore> mipsIOP;

    };

}

