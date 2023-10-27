#include <cassert>
#include <dlfcn.h>

#include <except.h>
#include <gpu/renders.h>
namespace zenith::gpu {
    std::unique_ptr<DriverCtx> loadVulkanDriver() {
        auto driver = std::make_unique<DriverCtx>();
        if (driver->isNil()) {
            *driver = dlopen("libvulkan.so", RTLD_NOW);
        }
        if (driver->isNil()) {
            throw GPUFail("No valid Vulkan driver was found on the host device");
        }
        driver->vulkanInstanceAddr = bit_cast<PFN_vkGetInstanceProcAddr>(
            dlsym(driver->getDrv(), "vkGetInstanceProcAddr"));
        assert(!driver->isNil());

        return driver;
    }
}

