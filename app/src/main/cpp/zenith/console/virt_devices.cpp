#include <console/virt_devices.h>

namespace zenith::console {
    VirtDevices::VirtDevices() {
        virtBlocks = std::make_shared<mio::GlobalMemory>();
        controller = std::make_shared<mio::DMAController>();

        controller->memoryChips = virtBlocks;

        mipsEER5900 = std::make_shared<eeiv::EEMipsCore>(controller);
        mipsIOP = std::make_shared<iop::IOMipsCore>(virtBlocks);
    }
}
