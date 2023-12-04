#pragma once
#include <EGL/egl.h>
#include <vulkan/vulkan_raii.hpp>
#include <gpu/vulcano/vram_allocator.h>
namespace cosmic::gpu {
    enum RenderApi : u8 {
        SoftwareSlow,
        HardwareVulkan,
        HardwareOpenGL
    };
    struct Gl3Render {
        EGLSurface surface;
    };
    constexpr u32 vkVersion{VK_API_VERSION_1_2};
    constexpr u32 appImplVersion{VK_MAKE_VERSION(1, 1, 4)};
    struct VkRender {
        VkRender() {}
        std::optional<vk::raii::SurfaceKHR> surface;
        std::unique_ptr<vulcano::VramManager> allocator;
    };
}