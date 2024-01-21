#pragma once

#include <common/types.h>
namespace cosmic::vu {
    class VectorUnit;
    class VuMicroExecutor {
    public:
        VuMicroExecutor(Ref<VectorUnit> vu) :
            vuMicro(vu) {}
        virtual u32 executeCode() = 0;
        virtual void setCurrentProgram(u32 crc) = 0;

        virtual std::pair<u32, u32> fetchPcInst() = 0;
        virtual ~VuMicroExecutor() = default;
    protected:
        Ref<VectorUnit> vuMicro;
    };
}
