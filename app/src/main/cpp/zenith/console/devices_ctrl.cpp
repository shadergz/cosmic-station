#include <console/virtual_devices.h>

namespace console {
    VirtualDevices::VirtualDevices(const std::shared_ptr<GlobalMemory>& blocks)
        : mipsEER5900(std::make_shared<eeiv::EmotionMIPS>(blocks))
        {}
}