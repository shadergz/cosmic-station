#include <ipu/decoder_fifo.h>

namespace cosmic::ipu {
    void DecoderFifo::resetDeck() {
        isCacheDirty = true;
        size = capacity = 0;
    }
}
