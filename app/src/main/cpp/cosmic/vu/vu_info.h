#pragma once

#include <common/types.h>
namespace cosmic::vu {
    class VectorUnit;
    class VuMicroExecutor {
    public:
        VuMicroExecutor(RawReference<VectorUnit> vu) :
            vuMicro(vu) {}
        virtual u32 executeCode() = 0;
        virtual void setCurrentProgram(u32 crc) = 0;

        virtual u32 fetchPcInst(u32 pc) = 0;
        virtual ~VuMicroExecutor() = default;
    protected:
        RawReference<VectorUnit> vuMicro;
    };
}
