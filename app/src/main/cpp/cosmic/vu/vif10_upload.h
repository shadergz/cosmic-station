#pragma once
#include <variant>
#include <common/types.h>
#include <gs/gif_bridge.h>

#include <console/intc.h>
#include <mio/dma_ctrl.h>
#include <vu/vif_fifo.h>
namespace cosmic::vu {
    class VifGifInterconnector {
    public:
        // A gateway to directly access the GIF
        VifGifInterconnector() = default;
        VifGifInterconnector(std::shared_ptr<gs::GifBridge> ark) : gif(ark) {}
        u8 getId() {
            if (gif)
                return 1;
            return 0;
        }
        std::shared_ptr<gs::GifBridge> gif{};
    };

    class VectorUnit;
    enum VifCommandStatus {
        CmdIdle,
        WaitingForData,
        DecodingCommand,
        DecAndTrans
    };
    enum VifStall {
        NotStalled,
        Ibt = 1,
        MskPath3 = 2,
        Stop = 4,
        Direct = 8,
        ForceBreak = 16
    };
    struct VifStatus {
        VifCommandStatus command;

        bool isStalledVss;
        bool isStalledVfs;
        bool isStalledIntVis;
        bool interrupt;

        bool vewWaitingVu;
        bool vgwWaitingGif;
        std::variant<bool, u8> error;
    };

    enum FifoStates {
        Cooking,
        GifDownloading // FIFO Reverse
    };

    class alignas(8) VifMalice {
    public:
        VifMalice() = default;
        VifMalice(VifMalice&) = delete;
        VifMalice(RawReference<VectorUnit> vector, VifGifInterconnector card);

        void update(u32 cycles);
        void resetVif();

        u32 getFifoFreeSpace() const {
            return fifo.size() / 4 - 1;
        }
        bool transferDmaData(os::vec quad, bool validateFreeSpace = false);

        VifGifInterconnector vif2gif{};
        std::shared_ptr<console::IntCInfra> interrupts;
        std::shared_ptr<mio::DmaController> dmac;

        RawReference<VectorUnit> vifVU;
        mio::DirectChannels vifId;
    private:
        u16 memMask{};
        u8 fifoSize{};
        // Amount of data not yet transferred in MPG/UNPACK
        u8 num;
        u32 mask,
            code;
        VifStatus vifS;
        VifFifo fifo;

        u8 isVifStalled{};

        // VIFn_ITOP: 0 and VIFn_ITOPS: 1 is packed here
        u16 tops[2];
        FifoStates fifoState;
    };
}
