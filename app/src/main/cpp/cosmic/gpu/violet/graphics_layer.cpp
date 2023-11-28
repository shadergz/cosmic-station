#include <gpu/violet/graphics_layer.h>
namespace cosmic::gpu::violet {
    static void startVulkanLayer(VioletLayer& layer) {
        layer.app = vk::raii::Context(layer.hardware->vulkanInstanceAddr);
        vk::raii::Instance instance{createVulkanInstance(*layer.app)};
    }

    VioletLayer::VioletLayer() {
        hardware = std::make_unique<RenderDriver>();
    }
    u32 VioletLayer::loadAllReferences(const RenderApi api) {
        if (api == HardwareVulkan) {
            establishGraphicsAPI = startVulkanLayer;
        } else if (api == HardwareOpenGL) {}
        return 0;
    }
}
