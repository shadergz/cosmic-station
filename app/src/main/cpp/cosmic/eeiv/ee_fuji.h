#pragma once

#include <common/types.h>
#include <eeiv/ee_info.h>
namespace cosmic::eeiv {
    class EEMipsCore;

    class EEExecutor {
    public:
        EEExecutor(EEMipsCore& mips)
            : mainMips(mips) {}
        virtual u32 executeCode() = 0;
        virtual u32 fetchPcInst() = 0;

        virtual ~EEExecutor() = default;

    protected:
        EEMipsCore& mainMips;
    };
}
