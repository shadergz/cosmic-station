#pragma once

#include <common/types.h>
#include <gs/gif_mandatory.h>

#include <console/intc.h>
#include <mio/dma_parallel.h>
namespace cosmic::vu {
    class VifVuInterconnector {
    public:
        VifVuInterconnector() = default;
        VifVuInterconnector(std::shared_ptr<gs::GifArk> ark) : gif(ark) {}
        u8 getId() {
            if (gif)
                return 1;
            return 0;
        }
        std::shared_ptr<gs::GifArk> gif{};
    };

    class VifMalice {
    public:
        VifMalice() = default;
        VifMalice(VifVuInterconnector card)
            : vu(card) {}
        VifVuInterconnector vu{};
        std::shared_ptr<console::INTCInfra> interrupts;
        std::shared_ptr<mio::DMAController> dmac;
    };
}
