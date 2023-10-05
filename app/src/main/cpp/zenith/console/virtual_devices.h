#pragma once

#include <memory>

#include <link/glb_memory.h>
#include <eeiv/ee_engine.h>
#include <iop/iop_core.h>
#include <kernel/hle.h>
namespace zenith::console {
    class VirtualDevices {
    public:
        VirtualDevices(const std::shared_ptr<link::GlobalMemory>& blocks);
        std::shared_ptr<eeiv::EEMipsCore> mipsEER5900;
        std::shared_ptr<iop::IOMipsCore> mipsIOP;
        std::unique_ptr<kernel::BiosHLE> biosHLE;

    };

}

