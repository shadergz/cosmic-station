#include <gs/gif_bridge.h>

namespace cosmic::gs {
    void GifBridge::queueReset() {
        fifoSize = {};
#if !NDEBUG
        memset(&gifFifo[0], 0xff, sizeof(gifFifo));
#endif
        __asm("eor v0.16b, v0.16b, v0.16b");
        for (u32 gifData{}; gifData < gifFifo.size(); ) {
            __asm("st1 {v0.16b}, [%0]" :: "r" (&gifFifo[gifData++]));
            __asm("st1 {v0.16b}, [%0]" :: "r" (&gifFifo[gifData++]));
            __asm("st1 {v0.16b}, [%0]" :: "r" (&gifFifo[gifData++]));
            __asm("st1 {v0.16b}, [%0]" :: "r" (&gifFifo[gifData++]));
        }
        fifoFront = std::ref(gifFifo[0]);
        fifoBack = std::ref(gifFifo[15]);
    }
    u32 GifBridge::queueGetSize() {
        if (fifoBack && fifoFront)
            fifoSize = static_cast<u32>(std::abs(&fifoBack - &fifoFront)) / sizeof(gifFifo[0]);
        // We can pre-load the array values into the L2 cache since we'll be accessing it shortly
        for (u32 preload{}; preload < gifFifo.size(); preload++)
            __asm("prfm pldl2keep, [%0]" :: "r" (&gifFifo[preload]));

        return fifoSize;
    }
}
