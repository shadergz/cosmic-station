#pragma once

#include <memory>

#include <engine/ee_core.h>
#include <iop/iop_core.h>
#include <vu/vecu.h>
#include <vu/vif10_upload.h>
#include <gs/synth_engine.h>
#include <ipu/ipu_core.h>
#include <mio/mem_pipe.h>
namespace cosmic::mio {
    class DmaController;
}
namespace cosmic::console {
    class INTCInfra;

    class VU01Pack {
    public:
        VU01Pack(std::shared_ptr<gs::GifArk> gif) {
            vifs[0] = vu::VifMalice(vpu0Cop2, vu::VifGifInterconnector{});
            vifs[1] = vu::VifMalice(vpu1DLO, vu::VifGifInterconnector{gif});
        }
        void populate(std::shared_ptr<INTCInfra> infra,
            std::shared_ptr<mio::DmaController> dma);

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
        void level2devsInit(std::shared_ptr<mio::MemoryPipe>& holder);
        std::shared_ptr<engine::EeMipsCore> mipsEeR5900;
        std::shared_ptr<iop::IoMipsCore> mipsIop;
        std::shared_ptr<ipu::IpuMpeg2> decoderMpeg12;

        std::shared_ptr<mio::GlobalMemory> virtBlocks;
        std::shared_ptr<VU01Pack> VUs;

        std::shared_ptr<gs::GifArk> gif;
        std::shared_ptr<gs::GsEngine> gs;
    };
}

