#include <console/virtual_devices.h>

namespace console {
    VirtualDevices::VirtualDevices(const std::shared_ptr<GlobalMemory>& blocks)
        : m_mipsEER5900(std::make_shared<eeiv::EEMipsCore>(blocks)),
          m_mipsIOP(std::make_shared<iop::IOMipsCore>())
        {}
}