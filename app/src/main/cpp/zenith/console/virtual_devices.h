#pragma once

#include <memory>

#include <link/global_memory.h>

#include <eeiv/ee_engine.h>
#include <iop/iop_core.h>

namespace zenith::console {
    class VirtualDevices {
    public:
        VirtualDevices(const std::shared_ptr<GlobalMemory>& blocks);
        std::shared_ptr<eeiv::EEMipsCore> m_mipsEER5900;
        std::shared_ptr<iop::IOMipsCore> m_mipsIOP;

    };

}

