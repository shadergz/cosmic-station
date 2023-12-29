#pragma once

#include <common/types.h>
#include <vk_mem_alloc.h>
namespace cosmic::gpu::vulcano {
    class GraphicsLayer;
    class VramManager {
    public:
        VramManager(RawReference<GraphicsLayer> gpu);
        ~VramManager();
    private:
        VmaAllocator vma{VK_NULL_HANDLE};
        RawReference<GraphicsLayer> graphics;
    };
}
