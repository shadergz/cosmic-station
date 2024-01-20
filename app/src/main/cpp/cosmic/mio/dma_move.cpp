#include <common/global.h>
#include <mio/dma_ctrl.h>
namespace cosmic::mio {
    static const std::array<std::string, 10> channelIds{
        "VIF0", "VIF1", "GIF", "IPU_FROM", "IPU_TO",
        "SIF0", "SIF1", "SIF2", "SPR_FROM", "SPR_TO"
    };


    void DmaController::advanceSrcDma(DirectChannels channel) {
        RawReference<DmaChannel> chan{};
        chan = std::ref(channels[channel]);
        chan->adr += 16;
        if (!chan->qwc) {
            throw MioFail("We don't need to continue anymore, caused by the channel: {}",
                channelIds.at(chan->index));
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