#pragma once
#include <variant>

#include <gpu/functions.h>
#include <gpu/renders.h>
#include <gpu/render_driver.h>
namespace cosmic::gpu {
    class GraphicsLayer : public GraphicsFunctionsRef {
    public:
        GraphicsLayer(RenderApi renderMode);
        std::variant<VkRender, Gl3Render> render;
        u32 reloadReferences();
        void updateLayer();

        std::unique_ptr<RenderDriver> hardware;
        std::optional<vk::raii::Context> app;
        std::optional<vk::raii::Instance> instance;

        RenderApi graphicsApi{};
    };
}
namespace cosmic::gpu::vulcano {
    vk::raii::Instance createVulkanInstance(const vk::raii::Context& context);
}