#include <cassert>
#include <dlfcn.h>

#include <common/global.h>
#include <common/except.h>
#include <gpu/render_driver.h>
namespace cosmic::gpu {
    void RenderDriver::pickUserRender(const RenderApi api, bool reload) {
        if (driver && !reload)
            return;
        switch (api) {
        case HardwareVulkan:
            if (!loadVulkanDriver()) {
                throw GpuFail("No instance of the Vulkan driver was found");
            }
            break;
        case SoftwareSlow:
        case HardwareOpenGL:
            break;
        }
    }
    bool RenderDriver::loadVulkanDriver() {
        auto serviceDriver{*(device->getStates()->customDriver)};
        auto appStorage{*(device->getStates()->appStorage)};
        if (driver)
            dlclose(std::exchange(driver, nullptr));
        if (serviceDriver.starts_with(appStorage)) {}
        if (!driver) {
            // Rolling back to the driver installed on the device
            driver = dlopen(serviceDriver.c_str(), RTLD_LAZY);
            if (!driver)
                driver = dlopen("libvulkan.so", RTLD_LAZY);
            if (!driver)
                throw GpuFail("No valid Vulkan driver was found on the host device");
        }
        vulkanInstanceAddr = bit_cast<PFN_vkGetInstanceProcAddr>(dlsym(driver, "vkGetInstanceProcAddr"));
        return driver && vulkanInstanceAddr;
    }
    RenderDriver::~RenderDriver() {
        if (driver)
            dlclose(driver);
        driver = {};
        vulkanInstanceAddr = {};
    }
}
