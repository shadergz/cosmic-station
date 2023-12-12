#pragma once

#include <common/types.h>
namespace cosmic::ipu {
    class DecoderFifo {
    public:
        void resetDeck();

        bool isCacheDirty;
        u16 size;
        u16 capacity;
    };
}