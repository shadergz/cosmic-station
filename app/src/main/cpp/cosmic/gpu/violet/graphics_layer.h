#pragma once
#include <variant>

#include <gpu/violet/functions.h>
#include <gpu/violet/renders.h>
#include <gpu/render_driver.h>
namespace cosmic::gpu::violet {
    class VioletLayer : public VioletPayload {
    public:
        VioletLayer();
        std::variant<VkRender, Gl3Render> render;
        u32 loadAllReferences(const RenderApi api);

        std::unique_ptr<RenderDriver> hardware;
        std::optional<vk::raii::Context> app;
    };
    vk::raii::Instance createVulkanInstance(const vk::raii::Context& context);
}