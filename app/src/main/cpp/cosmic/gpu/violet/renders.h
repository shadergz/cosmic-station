#pragma once
#include <EGL/egl.h>
#include <vulkan/vulkan_raii.hpp>

#include <common/types.h>
namespace cosmic::gpu::violet {
    enum RenderApi : u8 {
        HardwareVulkan,
        HardwareOpenGL
    };
    struct Gl3Render {
        EGLSurface surface;
    };
    constexpr u32 vkVersion{VK_API_VERSION_1_2};
    struct VkRender {
        VkRender() {}
        std::optional<vk::raii::SurfaceKHR> surface;
    };
}