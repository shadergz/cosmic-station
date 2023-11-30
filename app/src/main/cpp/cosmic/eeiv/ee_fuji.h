#pragma once

#include <common/types.h>
#include <eeiv/ee_info.h>
namespace cosmic::eeiv {
    class EeMipsCore;

    class EeExecutor {
    public:
        EeExecutor(EeMipsCore& mips)
            : mainMips(mips) {}
        virtual u32 executeCode() = 0;
        virtual u32 fetchPcInst() = 0;
        virtual ~EeExecutor() = default;
    protected:
        EeMipsCore& mainMips;
    };
}
