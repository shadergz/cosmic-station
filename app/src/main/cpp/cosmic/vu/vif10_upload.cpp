#include <vu/vif10_upload.h>

namespace cosmic::vu {
    void VifMalice::update(u32 cycles) {
    }

    void VifMalice::resetVif() {
        // VU1 has 4 times more memory than VU0
        if (vu.getId()) {
            memMask = 0x3ff;
            fifoSize = 64;
        } else {
            memMask = 0xff;
            fifoSize = 32;
        }
    }
}