#pragma once

#include <common/types.h>
namespace zenith::eeiv {
    class EEMipsCore;

    class EEExecutor {
    public:
        EEExecutor(EEMipsCore& mips)
            : mainMips(mips) {}
        virtual u32 executeCode() = 0;
        virtual ~EEExecutor() = default;

    protected:
        EEMipsCore& mainMips;
    };

}
