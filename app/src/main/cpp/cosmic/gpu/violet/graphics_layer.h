#pragma once
#include <variant>

#include <gpu/violet/functions.h>
#include <gpu/violet/renders.h>
#include <gpu/render_driver.h>
namespace cosmic::gpu::violet {
    class VioletLayer : public VioletFunctionsReferences {
    public:
        VioletLayer(RenderApi renderMode);
        std::variant<VkRender, Gl3Render> render;
        u32 reloadReferences();
        void updateLayer();

        std::unique_ptr<RenderDriver> hardware;
        std::optional<vk::raii::Context> app;

        RenderApi graphicsApi{};
    };
    vk::raii::Instance createVulkanInstance(const vk::raii::Context& context);
}