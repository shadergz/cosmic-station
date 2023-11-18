#pragma once

#include <variant>

#include <gpu/violet/function_payload.h>
#include <gpu/violet/vk_render.h>
#include <gpu/violet/gl3_render.h>
namespace cosmic::gpu::violet {
    class VioletLayer : public VioletPayload {
    public:
        VioletLayer() = default;
        std::variant<VkRender, Gl3Render> render;
    };
}
