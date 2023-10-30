#pragma once
#include <common/types.h>
#include <vulkan/vulkan.h>

namespace zenith::gpu {
    using LinkableObject = void*;

    class RenderEngine {
    public:
        RenderEngine() = default;
        ~RenderEngine();
        void operator=(LinkableObject devDriver) {
            driver = devDriver;
        }

        LinkableObject driver{};
        PFN_vkGetInstanceProcAddr vulkanInstanceAddr{};

        bool loadVulkanDriver();
    };
}
