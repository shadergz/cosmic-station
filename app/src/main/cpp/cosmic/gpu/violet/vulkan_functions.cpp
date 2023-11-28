#include <gpu/violet/graphics_layer.h>

namespace cosmic::gpu::violet {
    vk::raii::Instance createVulkanInstance(const vk::raii::Context& context) {
        /*
        vk::ApplicationInfo application{
            .pApplicationName = "Cosmic",
            .applicationVersion = 114,
            .pEngineName = "Emu.Graphics",
            .apiVersion = vkVersion
            };
        auto extensions{context.enumerateInstanceLayerProperties()};

        return vk::raii::Instance(context, vk::InstanceCreateInfo{
            .pApplicationInfo = &application,
        });
        */
        return vk::raii::Instance(context, nullptr);
    }
}
