#include <ee/cop0.h>
#include <mio/mem_pipe.h>

namespace cosmic::ee {
    bool CtrlCop::getCondition() {
        u32 stat{mio::bitBashing<u32>(dmac->performRead(0x1000e10)) & 0x3ff};
        u32 pcr{mio::bitBashing<u32>(dmac->performRead(0x1000e020)) & 0x3ff};
        return ((~pcr | stat) & 0x3ff) == 0x3ff;
    }
}
