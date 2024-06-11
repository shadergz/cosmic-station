#pragma once

#include <memory>

#include <ee/ee_core.h>
#include <iop/iop_core.h>
#include <vu/vecu.h>
#include <vu/vif10_upload.h>
#include <gs/gs_engine.h>
#include <ipu/ipu_core.h>
#include <mio/mem_pipe.h>
#include <iop/iop_dma.h>
#include <spu/sound_core.h>
namespace cosmic::mio {
    class DmaController;
}
namespace cosmic::console {
    class IntCInfra;

#pragma pack(push, 1)
    class Vu01Package {
    public:
        Vu01Package(std::shared_ptr<gs::GifBridge> gif) :
            vpu1Dlo(vu::VectorUnit({}, vu::VuWorkMemory(vpu1Space))),
            vpu0Cop2(vu::VectorUnit(vpu1Dlo, vpu0Space)) {

            vifs[0] = vu::VifMalice(vpu0Cop2, vu::VifGifInterconnector{});
            vifs[1] = vu::VifMalice(vpu1Dlo, vu::VifGifInterconnector{gif});
        }
        void populate(std::shared_ptr<IntCInfra> infra,
            std::shared_ptr<mio::DmaController> dma);

        std::array<vu::VifMalice, 2> vifs;
        // These two vector units could run in two modes, Parallel and Serial
        // Parallel mode: (CPU + VU0 <-> Scratchpad) + (VU1 <-> Main Memory) -> GIF
        // Serial mode: (MainMemory -> (CPU + VU0) -> Scratchpad -> VU1 -> GIF
        vu::VectorUnit vpu1Dlo;
        vu::VectorUnit vpu0Cop2;

        std::array<u8, 1024 * 4 * 4> vpu1Space[2];
        std::array<u8, 1024 * 4> vpu0Space[2];
    };
#pragma pack(pop)
    class VirtDevices {
    public:
        VirtDevices();
        void level2devsInit(std::shared_ptr<mio::MemoryPipe>& pipe);
        void level3devsInit(std::shared_ptr<mio::MemoryPipe>& pipe,
            std::shared_ptr<console::IntCInfra>& infra);

        std::shared_ptr<ee::EeMipsCore> eeR5900;
        std::shared_ptr<iop::IoMipsCore> mipsIop;
        std::shared_ptr<iop::IopDma> iopDma;
        std::shared_ptr<spu::Spu2> soundPu;

        std::shared_ptr<ipu::IpuMpeg2> decoderMpeg12;

        std::shared_ptr<mio::GlobalMemory> virtBlocks;
        std::shared_ptr<Vu01Package> VUs;

        std::shared_ptr<gs::GifBridge> gif;
        std::shared_ptr<gs::GsEngine> gs;
    private:
        std::shared_ptr<mio::DmaController> dma;
    };
}

