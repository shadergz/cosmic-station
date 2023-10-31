#pragma once

#include <gpu/violet/function_payload.h>
#include <gpu/violet/vk_render.h>
#include <gpu/violet/gl3_render.h>

namespace zenith::gpu::violet {
    class VioletLayer : public VioletPayload {
    public:
        VioletLayer() = default;

        union {
            VkRender vulkan{};
            Gl3Render gl;
        };
    };
}
