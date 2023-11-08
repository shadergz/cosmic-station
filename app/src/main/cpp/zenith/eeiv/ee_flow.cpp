#include <eeiv/ee_engine.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsign-conversion"
namespace zenith::eeiv {
    void EEMipsCore::verifyAndBranch(bool cond, i32 jumpRel) {
        if (cond) {
            isABranch = cond;
            eePC = *eePC + jumpRel + 4;
            delaySlot = 1;
        }
    }
}
#pragma clang diagnostic pop
