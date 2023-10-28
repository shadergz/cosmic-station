#include <gpu/hardware_render.h>
#include <global.h>

namespace zenith::gpu {
    RenderScene::RenderScene() {
        auto render = [this]() -> void {
            estUserRender();
        };
        device->getStates()->customDriver.listener = render;
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
