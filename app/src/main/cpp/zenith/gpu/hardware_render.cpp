#include <gpu/hardware_render.h>
#include <common/global.h>

namespace zenith::gpu {
    RenderScene::RenderScene() {
        device->getStates()->customDriver.listener = [this]() -> void {
            estUserRender();
        };
        driver = std::make_unique<RenderEngine>();
    }
    void RenderScene::estUserRender() {
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
