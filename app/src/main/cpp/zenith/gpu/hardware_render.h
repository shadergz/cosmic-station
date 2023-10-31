#pragma once

#include <common/types.h>
#include <gpu/renders.h>
namespace zenith::gpu {
    enum RenderApi : u8 {
        HardwareVulkan,
        HardwareOpenGL
    };

    class RenderScene {
    public:
        RenderScene();
        void pickUserRender(bool mandatory = false);
    private:
        RenderApi graphics{HardwareVulkan};
        std::unique_ptr<RenderEngine> driver;
    };
}