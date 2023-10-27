#include <gpu/hardware_render.h>

namespace zenith::gpu {
    RenderScene::RenderScene() {}

    void RenderScene::reloadUserDriver() {
        switch (selectedApi) {
        case HardwareVulkan:
            driverChan = loadVulkanDriver();
            if (!driverChan->vulkanInstanceAddr) {
                throw GPUFail("No instance of the Vulkan driver was found");
            }
        case HardwareOpenGL:
            break;
        }
    }
}
