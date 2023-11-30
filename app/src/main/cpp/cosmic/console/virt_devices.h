#pragma once

#include <memory>

#include <eeiv/ee_engine.h>
#include <iop/iop_core.h>
#include <vu/vecu.h>
#include <vu/vif10_upload.h>
namespace cosmic::mio {
    class DMAController;
}

namespace cosmic::console {
    class INTCInfra;

    class VU01Pack {
    public:
        VU01Pack(std::shared_ptr<gs::GifArk> gif) {
            vifs[0] = vu::VifVuInterconnector{};
            vifs[1] = vu::VifVuInterconnector{gif};
        }
        void populate(std::shared_ptr<INTCInfra> infra,
            std::shared_ptr<mio::DMAController> dma);

        vu::VifMalice vifs[2];
        // These two vector units could run in two modes, Parallel and Serial
        // Parallel mode: (CPU + VU0 <-> Scratchpad) + (VU1 <-> Main Memory) -> GIF
        // Serial mode: (MainMemory -> (CPU + VU0) -> Scratchpad -> VU1 -> GIF
        vu::VectorUnit vpu0Cop2;
        vu::VectorUnit vpu1DLO;
    };
    class VirtDevices {
    public:
        VirtDevices();
        std::shared_ptr<eeiv::EeMipsCore> mipsEER5900;
        std::shared_ptr<iop::IOMipsCore> mipsIOP;

        std::shared_ptr<mio::GlobalMemory> virtBlocks;
        std::shared_ptr<mio::DMAController> controller;
        std::shared_ptr<VU01Pack> VUs;

        std::shared_ptr<gs::GifArk> gif;
    };

}

