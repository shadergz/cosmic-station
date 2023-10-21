#pragma once

#include <types.h>
namespace zenith::eeiv {
class EEMipsCore;

class EEExecutor {
public:
    EEExecutor(EEMipsCore& intCpu) : rtIntCpu(intCpu) {}
    virtual u32 execCode() = 0;
    virtual ~EEExecutor() = default;

protected:
    EEMipsCore& rtIntCpu;
};

}
