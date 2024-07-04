#include <iop/iop_dma.h>
#include <spu/sound_core.h>

namespace cosmic::iop {
    void IopDma::resetIoDma() {
        activeChannel.reset();
        for (u64 chan{}; chan < channels.size(); chan++) {
            channels[chan] = {};
            channels[chan].index = static_cast<u8>(chan);
        }
    }
    void IopDma::pulse(u32 cycles) {
        if (!activeChannel)
            return;

        for (; cycles ; cycles--) {
            switch (activeChannel.channel) {
            case IopSpu2:
                pulseSpu2Chain();
                break;
            }
        }
    }
    void IopDma::pulseSpu2Chain() {
        std::array<u32, 2> packet;
        auto& spu2ch{channels[IopSpu2]};
        // When true, it means that we will write into the SPU2 device
        auto write2Spu{spu2ch.status.isFrom2Device};

        if (spu2ch.cyclesDelay) {
            spu2ch.cyclesDelay--;
            return;
        }
        if (spu2ch.cyclesDelay <= 0) {
            if (write2Spu) {
                packet[0] = ioDmaRead<u32>(spu2ch.addr);
                spu2->writeDmaData(packet[0]);
            } else {
                packet[1] = spu2->requestDmaData();
                ioDmaWrite<u32>(spu2ch.addr, packet[1]);
            }
            spu2ch.size--;
            spu2ch.addr += sizeof(u32);
            spu2ch.cyclesDelay = 3;
        }
        if (!spu2ch.size) {
            spu2ch.wordCount = 0;
        }
    }
}
