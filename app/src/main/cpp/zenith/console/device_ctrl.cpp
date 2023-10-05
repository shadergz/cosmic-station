#include <console/virtual_devices.h>

namespace zenith::console {
    VirtualDevices::VirtualDevices(const std::shared_ptr<link::GlobalMemory>& blocks)
        : mipsEER5900(std::make_shared<eeiv::EEMipsCore>(blocks)),
          mipsIOP(std::make_shared<iop::IOMipsCore>()) {}
}
