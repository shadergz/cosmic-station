#include <common/global.h>
#include <gpu/graphics_layer.h>

#include <fmt/format.h>
namespace cosmic::gpu::vulcano {
#ifndef NDEBUG
    VkDebugUtilsMessengerCreateInfoEXT createDebugInfo() {
        static VkDebugUtilsMessengerCreateInfoEXT debugMessengerInfo{};
        if (debugMessengerInfo.sType == VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT)
            return debugMessengerInfo;

        debugMessengerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugMessengerInfo.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugMessengerInfo.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT    |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugMessengerInfo.pfnUserCallback = debugMessagesCallback;
        debugMessengerInfo.pUserData = {};
        return debugMessengerInfo;
    }

    VkResult createDebugLayer(
        PFN_vkGetInstanceProcAddr getInstance,
        vk::raii::Instance& instance,
        const VkDebugUtilsMessengerCreateInfoEXT& pCreateInfo,
        const VkAllocationCallbacks& pAllocator,
        VkDebugUtilsMessengerEXT& pDebugMessenger) {

        auto vulkanFunc{reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>
            (getInstance(*instance, "vkCreateDebugUtilsMessengerEXT"))};
        if (!vulkanFunc)
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        return vulkanFunc(*instance, &pCreateInfo, &pAllocator, &pDebugMessenger);
    }
    void destroyDebugUtilsMessengerExt(
        PFN_vkGetInstanceProcAddr getInstance,
        vk::raii::Instance& instance,
        VkDebugUtilsMessengerEXT debugMessenger,
        const VkAllocationCallbacks& pAllocator) {

        auto vulkanFunc{reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(getInstance(*instance, "vkDestroyDebugUtilsMessengerEXT"))};
        if (vulkanFunc) {
            vulkanFunc(*instance, debugMessenger, &pAllocator);
        }
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL debugMessagesCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) {

        switch (messageSeverity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            user->info("(VK Validation Layer): {}", pCallbackData->pMessage); break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            user->error("(VK Validation Layer): {}", pCallbackData->pMessage); break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
            break;
        }

        const std::span<const VkDebugUtilsObjectNameInfoEXT> vkObjects{
            pCallbackData->pObjects, pCallbackData->objectCount};
        fmt::memory_buffer objAsStr{};
        for (auto& vkObj : vkObjects) {
            objAsStr.clear();
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wswitch"
            switch (vkObj.objectType) {
            case VK_OBJECT_TYPE_INSTANCE:
                fmt::format_to(back_inserter(objAsStr), "Instance, "); break;
            case VK_OBJECT_TYPE_PHYSICAL_DEVICE:
            case VK_OBJECT_TYPE_DEVICE:
                fmt::format_to(back_inserter(objAsStr), "Dev/Physical Dev, "); break;
            case VK_OBJECT_TYPE_QUEUE:
                fmt::format_to(back_inserter(objAsStr), "Queue, "); break;
            case VK_OBJECT_TYPE_COMMAND_BUFFER:
                fmt::format_to(back_inserter(objAsStr), "CMD Buffer, "); break;
            }
#pragma clang diagnostic pop
            fmt::format_to(back_inserter(objAsStr), "Handle: {}, ", vkObj.objectHandle);
            fmt::format_to(back_inserter(objAsStr), "Object Name: {}", vkObj.pObjectName);
            user->info("{}", fmt::join(objAsStr, ""));
        }
        return VK_FALSE;
    }
#endif
}