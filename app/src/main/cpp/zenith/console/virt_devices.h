#pragma once

#include <memory>

#include <eeiv/ee_engine.h>
#include <iop/iop_core.h>

namespace zenith::console {
    class VirtDevices {
    public:
        VirtDevices();

        std::shared_ptr<eeiv::EEMipsCore> mipsEER5900;
        std::shared_ptr<iop::IOMipsCore> mipsIOP;

        std::shared_ptr<mio::GlobalMemory> virtBlocks;
        std::shared_ptr<mio::DMAController> controller;
    };

}

