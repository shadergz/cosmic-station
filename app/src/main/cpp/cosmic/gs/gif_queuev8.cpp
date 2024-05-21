#include <range/v3/algorithm.hpp>
#include <gs/gif_bridge.h>

namespace cosmic::gs {
    void GifBridge::queueReset() {
        fifoSize = {};
#if !NDEBUG
        memset(&gifFifo[0], 0xff, sizeof(gifFifo));
#endif
        __asm("eor v0.16b, v0.16b, v0.16b");
#define STORE_PACKED_16B(addr)\
    __asm("st1 {v0.16b}, [%0]" :: "r" (addr))

        for (u32 gifData{}; gifData < gifFifo.size(); ) {
            STORE_PACKED_16B(&gifFifo[gifData++]);
            STORE_PACKED_16B(&gifFifo[gifData++]);
            STORE_PACKED_16B(&gifFifo[gifData++]);
            STORE_PACKED_16B(&gifFifo[gifData++]);
        }
        fifoFront = std::begin(gifFifo);
        ranges::fill(fifoArr, 0);
    }
    u64 GifBridge::queueFreePos() {
        u64 writable{};
        ranges::for_each(fifoArr, [&](const auto pos){
            if (!pos)
                writable++;
        });
        return writable;
    }
    os::vec GifBridge::queueConsume() {
        if (fifoFront > std::end(gifFifo)) {
            return {};
        }
        auto front{*fifoFront};
        fifoSize = static_cast<u64>(
            std::abs(fifoFront - std::begin(gifFifo)));
        fifoArr[fifoSize] = {};

        fifoFront++;
        return front;
    }

    u64 GifBridge::queueGetSize() {
        if (fifoFront) {
            fifoSize = static_cast<u64>(
                std::abs(fifoFront - std::begin(gifFifo)));
        }
        // We can pre-load the array values into the L2 cache since we'll be accessing it shortly
        for (u64 preload{}; preload < gifFifo.size(); preload++) {
            __asm("prfm pldl2keep, [%0]"::"r" (&gifFifo[preload]));
        }

        return fifoSize;
    }
}
