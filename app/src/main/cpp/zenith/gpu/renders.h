#pragma once
#include <types.h>
namespace zenith::gpu {
    using PFN_vkGetInstanceProcAddr = void*;

    struct DriverCtx {
        void* virtualAddress{};
        PFN_vkGetInstanceProcAddr vulkanInstanceAddr{};

        auto isNil() {
            return virtualAddress == nullptr;
        }
        auto getDrv() {
            return virtualAddress;
        }
        void operator=(void* driver) {
            virtualAddress = driver;
        }
    };
    std::unique_ptr<DriverCtx> loadVulkanDriver();
}
