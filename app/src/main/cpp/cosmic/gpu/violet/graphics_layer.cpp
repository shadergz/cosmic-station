#include <common/global.h>
#include <gpu/violet/graphics_layer.h>
namespace cosmic::gpu::violet {
    static void startVulkanLayer(raw_reference<VioletLayer> layer) {
        layer->app = vk::raii::Context(layer->hardware->vulkanInstanceAddr);
        vk::raii::Instance instance{createVulkanInstance(*layer->app)};
    }
    static void displayVersion(raw_reference<VioletLayer> layer) {
#if !defined(NDEBUG)
        if (layer->graphicsApi == HardwareVulkan) {
            u32 version{layer->app->enumerateInstanceVersion()};
            std::array<u32, 3> vkVA64{
                version >> 22 & 0x3ff, version >> 12 & 0x3ff, version & 0xfff};
            userLog->info("Vulkan version: {}", fmt::join(vkVA64, "."));
        } else {
            userLog->info("OpenGLES version: {}", eglQueryString(nullptr, EGL_VERSION));
        }
#endif
    }
    static const std::string hardwareApiNames(RenderApi api) {
        switch (api) {
        case RenderApi::HardwareOpenGL:
            return "OpenGLES";
        case RenderApi::HardwareVulkan:
            return "Vulkan Driver";
        }
        return "";
    }
    void VioletLayer::updateLayer() {
        u32 functions{reloadReferences()};
        u8 openGl{graphicsApi == HardwareOpenGL && functions == 0x0};
        u8 vulkan{graphicsApi == HardwareVulkan && functions == 0x1};

        if ((openGl + vulkan) == 0) {
            throw GPUFail("There is an error while attempting to load all {} layer functions", hardwareApiNames(graphicsApi));
        }
        prepareGraphicsApi(*this);
        displayApiVersion(*this);
    }
    VioletLayer::VioletLayer(RenderApi renderMode) : graphicsApi(renderMode) {
        hardware = std::make_unique<RenderDriver>();
        hardware->pickUserRender(renderMode);

        device->getStates()->customDriver.observer = [this]() {
            graphicsApi = violet::HardwareVulkan;
            hardware->pickUserRender(graphicsApi, true);
            updateLayer();
        };
        displayApiVersion = displayVersion;
    }
    u32 VioletLayer::reloadReferences() {
        u32 loaded{};
        if (graphicsApi == HardwareVulkan) {
            prepareGraphicsApi = startVulkanLayer;
        } else if (graphicsApi == HardwareOpenGL) {}
        if (prepareGraphicsApi)
            loaded++;
        return loaded;
    }
}
