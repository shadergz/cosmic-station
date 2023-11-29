#pragma once
#include <common/types.h>
#include <gpu/violet/renders.h>
namespace cosmic::gpu {
    using LinkableObject = void*;
    class RenderDriver {
    public:
        RenderDriver() = default;
        ~RenderDriver();
        void operator=(LinkableObject devDriver) {
            driver = devDriver;
        }
        LinkableObject driver{};
        PFN_vkGetInstanceProcAddr vulkanInstanceAddr{};

        void pickUserRender(const violet::RenderApi api, bool reload = false);
        bool loadVulkanDriver();
    };
}
