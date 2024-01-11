#include <spu/sound_processor.h>
namespace cosmic::spu {
    void Spu2::resetSound() {
        status = {};
        transferAddr = 0;
        currentAddr = 0;

    }
}