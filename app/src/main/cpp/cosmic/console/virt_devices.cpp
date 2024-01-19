#include <console/virt_devices.h>

namespace cosmic::console {
    VirtDevices::VirtDevices() {
        virtBlocks = std::make_shared<mio::GlobalMemory>();

        gif = std::make_shared<gs::GifBridge>();
        VUs = std::make_shared<Vu01Package>(gif);
    }
    void VirtDevices::level2devsInit(std::shared_ptr<mio::MemoryPipe>& pipe) {
        pipe->controller = std::make_shared<mio::DmaController>();
        pipe->controller->mapped = virtBlocks;

        eeR5900 = std::make_shared<engine::EeMipsCore>(pipe);
        mipsIop = std::make_shared<iop::IoMipsCore>(pipe);
        iopDma = std::make_shared<iop::IopDma>(pipe);

        decoderMpeg12 = std::make_shared<ipu::IpuMpeg2>(pipe->controller);
    }
    void VirtDevices::level3devsInit(
        std::shared_ptr<mio::MemoryPipe>& pipe,
        std::shared_ptr<console::IntCInfra> &infra) {
        soundPu = std::make_shared<spu::Spu2>(infra, iopDma, pipe);

        iop::IopDmaDevices ioDma{
            .sound = soundPu
        };
        iopDma->connectDevices(ioDma);

        mio::HardWithDmaCap caps{};
        caps.vif0 = std::ref(VUs->vifs[0]);
        caps.vif1 = std::ref(VUs->vifs[1]);
        caps.ee = eeR5900;

        pipe->controller->connectDevices(caps);
    }
    void Vu01Package::populate(std::shared_ptr<IntCInfra> infra,
        std::shared_ptr<mio::DmaController> dma) {

        vifs[0].dmac = dma;
        vifs[1].dmac = dma;
        vifs[0].interrupts = infra;
        vifs[1].interrupts = infra;
    }
}
