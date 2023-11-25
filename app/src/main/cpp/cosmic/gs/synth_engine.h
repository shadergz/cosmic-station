#pragma once

#include <common/types.h>
namespace cosmic::gs {
    enum GraphicsRender: u8 {
        Software,
        OpenGL,
        Vulkan,
    };
    class [[maybe_unused]] GSEngine {
    public:
        void resetGraphics();
    };
}
