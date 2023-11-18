#include <eeiv/c0/cop0.h>

namespace cosmic::eeiv::c0 {
    bool CoProcessor0::getCondition() {
        u32 stat{dmac->performRead(0x1000e010) & 0x3ff};
        u32 pcr{dmac->performRead(0x1000e020) & 0x3ff};
        return ((~pcr | stat) & 0x3ff) == 0x3ff;
    }
}
