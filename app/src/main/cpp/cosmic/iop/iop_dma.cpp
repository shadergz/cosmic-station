#include <iop/iop_dma.h>
#include <spu/sound_core.h>

namespace cosmic::iop {
    void IopDma::resetIoDma() {
        activeChannel.reset();
        for (u8 chan{}; chan < channels.size(); chan++) {
            channels[chan] = {};
            channels[chan].index = chan;
        }
    }
    void IopDma::pulse(u32 cycles) {
        if (!activeChannel)
            return;

        for (; cycles ; cycles--) {
            switch (activeChannel.channel) {
            case IopSpu2:
                pulseSpu2Chain(); break;
            }
        }
    }
    void IopDma::pulseSpu2Chain() {
        // When true, it means that we will write into the SPU2 device
        bool write2Spu;
        Ref<IopChan> channel;
        std::array<u32, 2> packet;

        channel = std::ref(channels[IopSpu2]);
        write2Spu = channel->status.isFrom2Device;

        if (channel->cyclesDelay) {
            channel->cyclesDelay--;
            return;
        }
        if (channel->cyclesDelay <= 0) {
            if (write2Spu) {
                packet[0] = ioDmaRead<u32>(channel->addr);
                spu2->writeDmaData(packet[0]);
            } else {
                packet[1] = spu2->requestDmaData();
                ioDmaWrite<u32>(channel->addr, packet[1]);
            }
            channel->size--;
            channel->addr += sizeof(u32);
            channel->cyclesDelay = 3;
        }
        if (!channel->size) {
            channel->wordCount = 0;
        }
    }
}
