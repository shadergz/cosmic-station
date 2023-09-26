#pragma once

#include <memory>

#include <link/GlobalMemory.h>
#include <eeiv/EE-Engine.h>
#include <iop/IOPCore.h>
namespace zenith::console {
    class VirtualDevices {
    public:
        VirtualDevices(const std::shared_ptr<GlobalMemory>& blocks);
        std::shared_ptr<eeiv::EEMipsCore> mipsEER5900;
        std::shared_ptr<iop::IOMipsCore> mipsIOP;

    };

}

