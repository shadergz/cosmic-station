#include <console/virt_devices.h>

namespace cosmic::console {
    VirtDevices::VirtDevices() {
        virtBlocks = std::make_shared<mio::GlobalMemory>();
        controller = std::make_shared<mio::DMAController>();

        gif = std::make_shared<gs::GifArk>();
        VUs = std::make_shared<VU01Pack>(gif);
        controller->memoryChips = virtBlocks;

        mipsEER5900 = std::make_shared<eeiv::EeMipsCore>(controller);
        mipsIOP = std::make_shared<iop::IoMipsCore>(virtBlocks);
    }
    void VU01Pack::populate(std::shared_ptr<INTCInfra> infra,
        std::shared_ptr<mio::DMAController> dma) {

        vifs[0].dmac = dma;
        vifs[1].dmac = dma;
        vifs[0].interrupts = infra;
        vifs[1].interrupts = infra;
    }
}
