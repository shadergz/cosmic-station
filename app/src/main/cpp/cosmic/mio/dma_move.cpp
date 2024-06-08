#include <common/global.h>
#include <mio/dma_ctrl.h>
namespace cosmic::mio {
    static const std::array<std::string, 10> channelsName{
        "Vif0", "Vif1", "Gif", "IpuFrom", "IpuTo",
        "Sif0", "Sif1", "Sif2", "SprFrom", "SprTo"
    };
    void DmaController::advanceSrcDma(DmaChannel& chan) {
        if (chan.request) {
        }
        chan.adr += 16;
        if (!chan.qwc) {
            throw MioErr("We don't need to continue anymore, caused by the channel: {}",
                channelsName.at(chan.index));
        }
        switch (chan.qwc) {
        case 0x1:
        default:
            chan.qwc--;
        }
        if (chan.isChan) {
            switch (chan.tagType) {
            case 1:
                chan.tagAdr = chan.adr;
                break;
            }
        }
    }
    void DmaController::switchChannel() {
        if (queued.size() != 0) {
            findNextChannel();
        }
    }
    void DmaController::findNextChannel() {
        if (hasOwner) {
            queued.push_back(hasOwner.getId());
            hasOwner.unselect();
        }
        if (queued.size() == 0) {
            user->info("The queue is empty, so let's proceed without adding a new channel");
            return;
        }
        auto nextChannel = queued.front();
        hasOwner.select(nextChannel);

        queued.pop_front();
    }
    void DmaController::checkStallOrActivateLater(DirectChannels channel) {
        auto& chan{channels[channel]};
        if (!(chan.request && chan.started)) {
            return;
        }

        bool checkForStall{};
        switch (status.stallDestChannel) {
        case 0x1:
            checkForStall = channel == Vif1; break;
        case 0x2:
            checkForStall = channel == Gif; break;
        case 0x3:
            checkForStall = channel == Sif1; break;
        }

        // Checks if the destination channel can be paused or should be, for a moment
        if (checkForStall && chan.hasStallDrain && chan.adr == stallAddress) {
            if (!chan.hasDmaStalled) {
                // At this point, we are waiting for the data in memory at the specified address
                // We cannot continue the transfer without first triggering an interrupt
                user->info("The channel {} is waiting ({} | {})",
                    channelsName[chan.index], chan.adr, stallAddress);
                raiseInt1();

                intStat.channelStat[DmaStall] = true;
                chan.hasDmaStalled = true;
            }
            queued.push_back(chan.index);
            return;
        }
        [[unlikely]] if (!hasOwner) {
            // The DMAC can now transfer data on the current channel
            hasOwner.select(chan.index);
        } else {
            queued.push_back(chan.index);
        }
    }
    void DmaController::issueADmacRequest(DirectChannels channel) {
        const auto previousReq{channels[channel].request};

        channels[channel].request = true;
        if (!previousReq) {
            checkStallOrActivateLater(channel);
        }
    }
    os::vec DmaController::performRead(u32 address) {
        os::vec fetched{};
        fetched = *dmaVirtSolver(address);
        return fetched;
    }
}