#include <console/VirtualDevices.h>

namespace zenith::console {
    VirtualDevices::VirtualDevices(const std::shared_ptr<GlobalMemory>& blocks)
        : mipsEER5900(std::make_shared<eeiv::EEMipsCore>(blocks)),
          mipsIOP(std::make_shared<iop::IOMipsCore>()) {}
}