#pragma once
#include <variant>
#include <common/types.h>
#include <gs/gif_mandatory.h>

#include <console/intc.h>
#include <mio/dma_parallel.h>
#include <vu/vif_fifo.h>
namespace cosmic::vu {
    class VifGifInterconnector {
    public:
        // A gateway to directly access the GIF
        VifGifInterconnector() = default;
        VifGifInterconnector(std::shared_ptr<gs::GifArk> ark) : gif(ark) {}
        u8 getId() {
            if (gif)
                return 1;
            return 0;
        }
        std::shared_ptr<gs::GifArk> gif{};
    };

    class VectorUnit;
    enum VifCommandStatus {
        CmdIdle,
        WaitingForData,
        DecodingCommand,
        DecAndTrans
    };
    struct VifStatus {
        VifCommandStatus command;

        bool isStalledVss;
        bool isStalledVfs;
        bool isStalledIntVis;
        bool interrupt;
        std::variant<bool, u8> error;
    };

    enum FifoStates {
        Cooking,
        GifDownloading // FIFO Reverse
    };

    class VifMalice {
    public:
        VifMalice() = default;
        VifMalice(VifMalice&) = delete;
        VifMalice(raw_reference<VectorUnit> vector, VifGifInterconnector card);

        void update(u32 cycles);
        void resetVif();

        VifGifInterconnector vif2gif{};
        std::shared_ptr<console::INTCInfra> interrupts;
        std::shared_ptr<mio::DMAController> dmac;

        raw_reference<VectorUnit> vifVU;
    private:
        u16 memMask{};
        u8 fifoSize{};
        // Amount of data not yet transferred in MPG/UNPACK
        u8 num;
        u32 mask,
            code;
        VifStatus vifS;
        VifFifo fifo;

        // VIFn_ITOP: 0 and VIFn_ITOPS: 1 is packed here
        u16 tops[2];
        FifoStates fifoState;
    };
}
