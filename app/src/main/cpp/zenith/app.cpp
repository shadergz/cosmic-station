#include <app.h>

namespace zenith {
std::unique_ptr<java::JvmManager> device;
std::shared_ptr<GlobalLogger> userLog;
std::shared_ptr<CoreApplication> zenithApp;

CoreApplication::CoreApplication()
    : virBlocks(std::make_shared<link::GlobalMemory>()),
      simulated(std::make_shared<console::VirtualDevices>(virBlocks)) {

    auto osState{device->getServiceState()};
    osState->syncAllSettings();

    vm = std::make_unique<console::EmuVM>(device->android, virBlocks, simulated);
    vm->resetVM();
}

std::shared_ptr<kernel::KernelsGroup> CoreApplication::getKernelsGroup() {
    auto group{vm->biosHLE->group};
    return group;
}
}
