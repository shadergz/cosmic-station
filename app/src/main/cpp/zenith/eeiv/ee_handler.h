#pragma once

#include <impl_types.h>

namespace zenith::eeiv {
    class EEMipsCore;

    class EEExecutor {
    public:
        EEExecutor(EEMipsCore& intCpu)
          : m_rtIntCpu(intCpu)
          {}
        virtual u32 execCode() = 0;
    protected:
        virtual ~EEExecutor() = default;

        EEMipsCore& m_rtIntCpu;
    };

}
