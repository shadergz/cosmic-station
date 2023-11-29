#include <gpu/violet/graphics_layer.h>

namespace cosmic::gpu::violet {
    vk::raii::Instance createVulkanInstance(const vk::raii::Context& context) {
        vk::ApplicationInfo application{
            .pApplicationName = "Cosmic",
            .applicationVersion = VK_MAKE_VERSION(1, 1, 4),
            .pEngineName = "Vulkan.Violet",
            .engineVersion = VK_MAKE_VERSION(1, 0, 0),
            .apiVersion = vkVersion
        };
        auto extensions{context.enumerateInstanceLayerProperties()};

        return vk::raii::Instance(context, vk::InstanceCreateInfo{
            .pApplicationInfo = &application,
        });
    }
}
