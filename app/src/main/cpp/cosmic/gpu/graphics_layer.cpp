#include <common/global.h>
#include <gpu/graphics_layer.h>
namespace cosmic::gpu {
    static void displayVersion(Ref<GraphicsLayer> gpu) {
#if !defined(NDEBUG)
        if (gpu->graphicsApi == HardwareVulkan) {
            u32 version{gpu->app->enumerateInstanceVersion()};
            std::array<u32, 3> vkVA64{
                version >> 22 & 0x3ff, version >> 12 & 0x3ff, version & 0xfff};
            user->info("Vulkan version: {}", fmt::join(vkVA64, "."));
        } else {
            user->info("OpenGLES version: {}", eglQueryString(nullptr, EGL_VERSION));
        }
#endif
    }
    static const std::string apiNames(RenderApi api) {
        switch (api) {
        case RenderApi::HardwareOpenGL:
            return "OpenGLES";
        case RenderApi::HardwareVulkan:
            return "Vulkan Driver";
        case SoftwareSlow:
            return "Software (CPU)";
        }
        return "";
    }
    void GraphicsLayer::updateLayer() {
        u32 functions{loadGraphicsApi()};
        u8 openGl{graphicsApi == HardwareOpenGL && functions == 0x0};
        u8 vulkan{graphicsApi == HardwareVulkan && functions == 0x1};

        if ((openGl + vulkan) == 0) {
            throw GpuFail("There is an error while attempting to load all {} layer functions", apiNames(graphicsApi));
        }
        prepareGraphicsApi(*this);
        displayApiVersion(*this);
    }
    GraphicsLayer::GraphicsLayer(RenderApi renderMode) : graphicsApi(renderMode) {
        backend = std::make_unique<RenderDriver>();
        backend->pickUserRender(renderMode);

        device->getStates()->addObserver(os::GpuCustomDriver, [&]() {
            graphicsApi = HardwareVulkan;
            backend->pickUserRender(graphicsApi, true);
            updateLayer();
        });
        displayApiVersion = displayVersion;
    }
    static void startVulkanLayer(Ref<GraphicsLayer> gpu) {
        gpu->app.reset();
        gpu->instance.reset();

        auto getInstance{gpu->backend->vulkanInstanceAddr};
        gpu->app = vk::raii::Context(getInstance);
        gpu->instance = vulcano::createVulkanInstance(*gpu->app, gpu->haveValidation);

        struct vulcano::PhysicalDevice vulkanGpu{
            vulcano::createPhysicalDevice(*gpu->instance)};

        gpu->vkDev = std::move(vulkanGpu.gpuUser);
        gpu->deviceInfo = vulkanGpu.info;
        gpu->queueFamilyId = vulkanGpu.desiredQueueId;
#ifndef NDEBUG
        if (gpu->haveValidation) {
            auto debugInfoMsg{vulcano::createDebugInfo()};
            vulcano::createDebugLayer(
                getInstance, *gpu->instance, debugInfoMsg, {}, gpu->debugMessenger);
        }
#endif
    }
    u32 GraphicsLayer::loadGraphicsApi() {
        u32 loaded{};
        if (graphicsApi == HardwareVulkan) {
            prepareGraphicsApi = startVulkanLayer;
        } else if (graphicsApi == HardwareOpenGL) {}
        if (prepareGraphicsApi)
            loaded++;
        return loaded;
    }
    GraphicsLayer::~GraphicsLayer() {
#ifndef NDEBUG
        if (debugMessenger && haveValidation) {
            vulcano::destroyDebugUtilsMessengerExt(
                backend->vulkanInstanceAddr, *instance, debugMessenger, {});
        }
#endif
    }
}
