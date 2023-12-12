#include <console/virt_devices.h>

namespace cosmic::console {
    VirtDevices::VirtDevices() {
        virtBlocks = std::make_shared<mio::GlobalMemory>();

        gif = std::make_shared<gs::GifArk>();
        VUs = std::make_shared<VU01Pack>(gif);
        mipsIop = std::make_shared<iop::IoMipsCore>(virtBlocks);
    }
    void VirtDevices::level2devsInit(std::shared_ptr<mio::MemoryPipe>& holder) {
        holder->controller = std::make_shared<mio::DmaController>();
        holder->controller->memoryMapped = virtBlocks;

        mipsEeR5900 = std::make_shared<engine::EeMipsCore>(holder);
        decoderMpeg12 = std::make_shared<ipu::IpuMpeg2>(holder->controller);
    }
    void VU01Pack::populate(std::shared_ptr<INTCInfra> infra,
        std::shared_ptr<mio::DmaController> dma) {

        vifs[0].dmac = dma;
        vifs[1].dmac = dma;
        vifs[0].interrupts = infra;
        vifs[1].interrupts = infra;
    }
}
