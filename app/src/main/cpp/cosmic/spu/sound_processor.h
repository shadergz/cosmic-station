#pragma once
#include <common/types.h>
namespace cosmic {
    namespace console {
        class IntCInfra;
    }
    namespace iop {
        class IopDma;
    }
}

namespace cosmic::spu {
    struct SpuStatus {
        bool dmaReady;
        bool dmaBusy;
    };
    class Spu2 {
    public:
        Spu2(std::shared_ptr<console::IntCInfra>& infra, std::shared_ptr<iop::IopDma>& ioDma) :
            intc(infra), dmac(ioDma)
            {
        }
        SpuStatus status;

        void resetSound();
        u32 transferAddr;
        u32 currentAddr;

    private:
        std::shared_ptr<console::IntCInfra> intc;
        std::shared_ptr<iop::IopDma> dmac;
    };
}