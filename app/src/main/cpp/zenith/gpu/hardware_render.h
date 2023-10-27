#pragma once

#include <types.h>
#include <gpu/renders.h>
namespace zenith::gpu {
    enum RenderApi : u8 {
        HardwareVulkan,
        HardwareOpenGL,
    };

    class RenderScene {
    public:
        RenderScene();
        void reloadUserDriver();
    private:
        RenderApi selectedApi;
        std::unique_ptr<DriverCtx> driverChan;
    };
}