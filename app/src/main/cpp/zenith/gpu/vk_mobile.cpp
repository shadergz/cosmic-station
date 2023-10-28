#include <cassert>
#include <dlfcn.h>

#include <global.h>
#include <except.h>
#include <gpu/renders.h>
namespace zenith::gpu {
    bool RenderEngine::loadVulkanDriver() {
        auto serviceDriver{*(device->getStates()->customDriver)};
        auto appStorage{*(device->getStates()->appStorage)};

        if (driver)
            dlclose(std::exchange(driver, nullptr));

        if (serviceDriver.starts_with(appStorage)) {}

        if (!driver) {
            // Rolling back to the driver installed on the device
            driver = dlopen(serviceDriver.c_str(), RTLD_NOW);
            if (!driver)
                throw GPUFail("No valid Vulkan driver was found on the host device");
        }
        vulkanInstanceAddr = bit_cast<PFN_vkGetInstanceProcAddr>(
            dlsym(driver, "vkGetInstanceProcAddr"));
        assert(driver && vulkanInstanceAddr);

        return true;
    }
}
