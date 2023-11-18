#include <gpu/hw_render.h>
#include <common/global.h>

namespace cosmic::gpu {
    RenderScene::RenderScene() {
        device->getStates()->customDriver.observer = [this]() {
            pickUserRender(true);
        };
        driver = std::make_unique<RenderDriver>();
    }
    void RenderScene::pickUserRender(bool mandatory) {
        if (driver && !mandatory)
            return;

        switch (graphics) {
        case HardwareVulkan:
            if (!driver->loadVulkanDriver()) {
                throw GPUFail("No instance of the Vulkan driver was found");
            }
        case HardwareOpenGL:
            break;
        }
    }
}
