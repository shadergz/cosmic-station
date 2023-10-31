#pragma once

#include <optional>
#include <vulkan/vulkan_raii.hpp>

namespace zenith::gpu::violet {
    struct VkRender {
        VkRender() {}
        std::optional<vk::raii::SurfaceKHR> surface;
    };
}

