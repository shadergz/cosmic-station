#pragma once

#include <common/types.h>
#include <vk_mem_alloc.h>
namespace cosmic::gpu::vulcano {
    class GraphicsLayer;
    class VramManager {
    public:
        VramManager(Ref<GraphicsLayer> gpu);
        ~VramManager();
    private:
        VmaAllocator vma{VK_NULL_HANDLE};
        Ref<GraphicsLayer> graphics;
    };
}
