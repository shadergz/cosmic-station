#include <range/v3/algorithm.hpp>
#include <gpu/graphics_layer.h>

namespace cosmic::gpu::vulcano {
    vk::raii::Instance createVulkanInstance(const vk::raii::Context& context) {
        vk::ApplicationInfo application{
            .pApplicationName = "Cosmic",
            .applicationVersion = appImplVersion,
            .apiVersion = vkVersion
        };
        auto extensions{context.enumerateInstanceExtensionProperties()};
        constexpr std::array<const char*, 2> requiredExtensions{
            "VK_KHR_surface",
            "VK_KHR_android_surface", // Provide a way to connect/refer a VkSurfaceKHR as a ANativeWindows
        };
        for (const auto required : requiredExtensions) {
            if (!ranges::any_of(extensions, [&](const auto& available) {
                return std::string_view(available.extensionName).starts_with(required);
            })) {
                throw GpuFail("Couldn't find a Vulkan extension with name {}", required);
            }
        }

        return vk::raii::Instance(context, vk::InstanceCreateInfo{
            .pApplicationInfo = &application,
            .enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()),
            .ppEnabledExtensionNames = requiredExtensions.data(),
        });
    }
}
