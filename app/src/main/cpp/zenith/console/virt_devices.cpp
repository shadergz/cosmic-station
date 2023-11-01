#include <console/virt_devices.h>

namespace zenith::console {
    VirtDevices::VirtDevices(std::shared_ptr<link::GlobalMemory>& blocks)
        : mipsEER5900(std::make_shared<eeiv::EEMipsCore>(blocks)),
          mipsIOP(std::make_shared<iop::IOMipsCore>()) {}
}
