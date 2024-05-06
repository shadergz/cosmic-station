#include <common/global.h>
#include <mio/dma_ctrl.h>
namespace cosmic::mio {
    static const std::array<const char*, 10> channelsName{
        "Vif0", "Vif1", "Gif", "IpuFrom", "IpuTo",
        "Sif0", "Sif1", "Sif2", "SprFrom", "SprTo"};
    void DmaController::advanceSrcDma(Ref<DmaChannel>& chan) {
        if (chan->request) {
        }
        chan->adr += 16;
        if (!chan->qwc) {
            throw MioErr("We don't need to continue anymore, caused by the channel: {}", channelsName.at(chan->index));
        }
        switch (chan->qwc) {
        case 0x1:
        default:
            chan->qwc--;
        }
        if (chan->isChan) {
            switch (chan->tagType) {
            case 1:
                chan->tagAdr = chan->adr; break;
            }
        }
    }
    void DmaController::switchChannel() {
        if (queued.size() != 0) {
            findNextChannel();
        }
    }
    void DmaController::findNextChannel() {
        if (hasOwner.locked) {
            queued.push_back(channels[hasOwner.id]);
            hasOwner.unselect();
        }
        std::list<DmaChannel>::value_type channel;
        if (queued.size() == 0) {
            user->info("The queue is empty, so let's proceed without adding a new channel");
            return;
        }

        channel = queued.front();
        hasOwner.select(channel.index);

        queued.pop_front();
    }
    void DmaController::issueADmacRequest(DirectChannels channel) {
        channels[channel].request = true;
    }
    os::vec DmaController::performRead(u32 address) {
        os::vec fetched{};
        fetched = *dmaVirtSolver(address);
        return fetched;
    }
}