#pragma once
#include <variant>

#include <gpu/functions.h>
#include <gpu/renders.h>
#include <gpu/render_driver.h>
namespace cosmic::gpu {
    class GraphicsLayer : public GraphicsFunctionsRef {
    public:
        GraphicsLayer(RenderApi renderMode);
        ~GraphicsLayer();
        std::variant<VkRender, Gl3Render> render;
        u32 reloadReferences();
        void updateLayer();

        std::unique_ptr<RenderDriver> backend;
        std::optional<vk::raii::Context> app;
        std::optional<vk::raii::Instance> instance;

        std::optional<vk::raii::Device> vkDev;
        std::optional<vk::DeviceCreateInfo> deviceInfo;

        bool haveValidation{};
#ifndef NDEBUG
        VkDebugUtilsMessengerEXT debugMessenger;
#endif
        u32 queueFamilyId{};

        RenderApi graphicsApi{};
    };
}
namespace cosmic::gpu::vulcano {
    constexpr auto invQueueId{std::numeric_limits<u32>::max()};
    struct PhysicalDevice {
        PhysicalDevice() {}
        std::optional<vk::raii::PhysicalDevice> physicalDev;
        std::optional<vk::raii::Device> gpuUser;
        vk::DeviceCreateInfo info{};
        u32 desiredQueueId{invQueueId};
    };

    vk::raii::Instance createVulkanInstance(const vk::raii::Context& context);
    PhysicalDevice createPhysicalDevice(vk::raii::Instance& vki, bool& haveValidationLayer);

#ifndef NDEBUG
    VkResult createDebugLayer(
        PFN_vkGetInstanceProcAddr getInstance,
        vk::raii::Instance& instance,
        const VkDebugUtilsMessengerCreateInfoEXT& pCreateInfo,
        const VkAllocationCallbacks& pAllocator,
        VkDebugUtilsMessengerEXT& pDebugMessenger);

    void destroyDebugUtilsMessengerExt(
        PFN_vkGetInstanceProcAddr getInstance,
        vk::raii::Instance& instance,
        VkDebugUtilsMessengerEXT debugMessenger,
        const VkAllocationCallbacks& pAllocator);

    VKAPI_ATTR VkBool32 VKAPI_CALL debugMessagesCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

    VkDebugUtilsMessengerCreateInfoEXT createDebugInfo();
#endif
}