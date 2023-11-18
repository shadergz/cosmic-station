#pragma once

#include <optional>
#include <vulkan/vulkan_raii.hpp>

namespace cosmic::gpu::violet {
    struct VkRender {
        VkRender() {}
        std::optional<vk::raii::SurfaceKHR> surface;
    };
}

