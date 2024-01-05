#include <common/global.h>
#include <gpu/graphics_layer.h>
namespace cosmic::gpu {
    static void displayVersion(GraphicsLayer& layer) {
#if !defined(NDEBUG)
        if (layer.graphicsApi == HardwareVulkan) {
            u32 version{layer.app->enumerateInstanceVersion()};
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
        case SoftwareSlow:
            return "Software CPU";
        }
        return "";
    }
    void GraphicsLayer::updateLayer() {
        u32 functions{reloadReferences()};
        u8 openGl{graphicsApi == HardwareOpenGL && functions == 0x0};
        u8 vulkan{graphicsApi == HardwareVulkan && functions == 0x1};

        if ((openGl + vulkan) == 0) {
            throw GpuFail("There is an error while attempting to load all {} layer functions",
                hardwareApiNames(graphicsApi));
        }
        prepareGraphicsApi(*this);
        displayApiVersion(*this);
    }
    GraphicsLayer::GraphicsLayer(RenderApi renderMode) : graphicsApi(renderMode) {
        hardware = std::make_unique<RenderDriver>();
        hardware->pickUserRender(renderMode);

        device->getStates()->addObserver(os::GpuCustomDriver, [this](JNIEnv* os) {
            graphicsApi = HardwareVulkan;
            hardware->pickUserRender(graphicsApi, true);
            updateLayer();
        });
        displayApiVersion = displayVersion;
    }
    static void startVulkanLayer(GraphicsLayer& layer) {
        layer.app = vk::raii::Context(layer.hardware->vulkanInstanceAddr);
        layer.instance = vulcano::createVulkanInstance(*layer.app);

        auto vulkanGpu{vulcano::createPhysicalDevice(*layer.instance)};
        layer.vkDev = std::move(vulkanGpu.gpuUser);
        layer.deviceInfo = vulkanGpu.info;
        layer.queueFamilyId = vulkanGpu.desiredQueueId;
    }

    u32 GraphicsLayer::reloadReferences() {
        u32 loaded{};
        if (graphicsApi == HardwareVulkan) {
            prepareGraphicsApi = startVulkanLayer;
        } else if (graphicsApi == HardwareOpenGL) {}
        if (prepareGraphicsApi)
            loaded++;
        return loaded;
    }
}
