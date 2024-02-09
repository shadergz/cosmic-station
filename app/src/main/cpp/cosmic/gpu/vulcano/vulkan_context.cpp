#include <range/v3/algorithm.hpp>
#include <gpu/graphics_layer.h>

namespace cosmic::gpu::vulcano {
    static vk::QueueFlags requiredCapabilities{
        vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute};
    std::array<const char*, 1> deviceExtensions{
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    std::vector<const char*> requiredLayers{};

    PhysicalDevice createPhysicalDevice(vk::raii::Instance& vki) {
        PhysicalDevice physical{};
        // Enumerating Vulkan devices present on the device
        auto devices{vki.enumeratePhysicalDevices()};
        if (devices.empty()) {
            throw GpuErr("No Vulkan device found on your device");
        }
        // Selecting and reading properties from the device
        for (const auto& dev : devices) {
            physical.physicalDev = dev;
            auto properties{physical.physicalDev->getQueueFamilyProperties()};
            auto props{properties.begin()};
            while (props != properties.end()) {
                if ((props->queueFlags & requiredCapabilities) != requiredCapabilities)
                    continue;
                physical.desiredQueueId = static_cast<u32>(
                    std::distance(std::begin(properties), props));
                break;
            }

            if (physical.desiredQueueId != invQueueId)
                break;
        }
        if (physical.desiredQueueId == invQueueId) {
            throw GpuErr("Unable to find a valid queue family on the device");
        }
        u32 queueCount{1};
        u32 enableLayerCount{};
        f32 queuePriority{0.f};
        static std::array<vk::PhysicalDeviceFeatures, 1> features{};

        vk::DeviceQueueCreateInfo queueInfo{
            .queueFamilyIndex = physical.desiredQueueId,
            .queueCount = queueCount,
            .pQueuePriorities = &queuePriority
        };

        features[0].samplerAnisotropy = VK_TRUE;
        physical.info = vk::DeviceCreateInfo{
            .queueCreateInfoCount = queueCount,
            .pQueueCreateInfos = &queueInfo,
            .enabledLayerCount = enableLayerCount,
            .enabledExtensionCount = deviceExtensions.size(),
            .ppEnabledExtensionNames = deviceExtensions.data(),
            .pEnabledFeatures = features.data()
        };
        physical.gpuUser = vk::raii::Device(*physical.physicalDev, physical.info);

        if (!physical.gpuUser) {
            throw GpuErr("Anomaly detected, Vulkan device not created");
        }
        return physical;
    }

    std::vector<const char*> requiredExtensions{
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_KHR_ANDROID_SURFACE_EXTENSION_NAME, // Provide a way to connect/refer a VkSurfaceKHR as a ANativeWindows
    };
    vk::raii::Instance createVulkanInstance(const vk::raii::Context& context, bool& haveValidationLayer) {
        static vk::ApplicationInfo application{
            .pApplicationName = "Cosmic",
            .applicationVersion = appImplVersion,
            .apiVersion = vkVersion
        };
#ifndef NDEBUG
        haveValidationLayer = true;
        requiredLayers.push_back("VK_LAYER_KHRONOS_validation");
        requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#else
        haveValidationLayer = {};
#endif

        auto extensions{context.enumerateInstanceExtensionProperties()};
        for (const auto required : requiredExtensions) {
            if (!ranges::any_of(extensions, [&](const auto& available) {
                return std::string_view(available.extensionName).starts_with(required);
            })) {
#ifndef NDEBUG
                if (std::string(required) == VK_EXT_DEBUG_UTILS_EXTENSION_NAME) {
                    requiredLayers.pop_back();
                    requiredExtensions.pop_back();
                    haveValidationLayer = false;
                } else {
                    throw GpuErr("Couldn't find a Vulkan extension with name {}", required);
                }
#else
                throw GpuFail("Couldn't find a Vulkan extension with name {}", required);
#endif
            }
        }
#ifndef NDEBUG
        static auto debugMessenger{createDebugInfo};
        return vk::raii::Instance(context, vk::InstanceCreateInfo{
            .pNext = &debugMessenger,
            .pApplicationInfo = &application,
            .enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
            .ppEnabledLayerNames = requiredLayers.data(),
            .enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()),
            .ppEnabledExtensionNames = requiredExtensions.data(),
        });
#else
        return vk::raii::Instance(context, vk::InstanceCreateInfo{
            .pApplicationInfo = &application,
            .enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
            .ppEnabledLayerNames = requiredLayers.data(),
            .enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()),
            .ppEnabledExtensionNames = requiredExtensions.data(),
        });
#endif
    }
}
