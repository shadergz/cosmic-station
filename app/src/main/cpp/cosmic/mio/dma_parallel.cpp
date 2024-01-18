#include <range/v3/algorithm.hpp>
#include <cpu/cyclic32.h>
#include <mio/dma_parallel.h>
#include <vu/vif10_upload.h>
namespace cosmic::mio {
    void DmaController::connectDevices(HardWithDmaCap& devices) {
        hw.vif0 = devices.vif0;
        hw.vif1 = devices.vif1;
    }
    DmaController::DmaController() {
        queued.resize(channels.size());
        getStagedChannel = [&](const DmaChannelId requested) {
            std::list<DmaChannel>::iterator b, e;
            b = std::begin(queued);
            e = std::end(queued);

            bool isValid{};
            std::array<u32, 2> crc;

            for (; b != e && !isValid; b++) {
                std::array<DmaChannel, 9>::value_type local{};
                std::array<DmaChannel, 9>::value_type staged{};
                staged = *b;
                local = channels[requested];

                crc[0] = cpu::check32({BitCast<u8*>(&staged), sizeof(staged)});
                crc[1] = cpu::check32({BitCast<u8*>(&local), sizeof(local)});
                isValid = crc[0] == crc[1];
            }
            if (isValid)
                return b;
            return e;
        };

        std::list<DmaChannel> empty{};
        queued.swap(empty);
    }

    void DmaController::resetMa() {
        status.isDmaEnabled = false;
        // According to DobieStation, it is a requirement for the SCPH-39001 BIOS
        ir.dicr = 0x1201;

        for (u8 dmIn{}; dmIn < 9; dmIn++) {
            channels[dmIn].index = dmIn;
            switch (dmIn) {
            // These channels do not have a specified FIFO; thus, they can be executed as soon as they arrive
            case SprFrom:
            case SprTo:
            case IpuTo:
            case Sif1:
            case Vif0:
            case Vif1:
                channels[dmIn].request = true;
            }
            channels[dmIn].request = false;
        }
        priorityCtrl = 0;
        // stallAddress = 0;
        highCycles = 0;
        queued.clear();
    }
    void DmaController::pulse(u32 cycles) {
        bool masterEnb{status.isDmaEnabled};
        if (!masterEnb || ir.busError)
            return;

        if (hasOwner)
            highCycles += cycles;
        u32 reduce{};

        for (; hasOwner && highCycles > 0; ) {
            RawReference<DmaChannel> tv{};
            tv = std::ref(channels.at(hasOwner.id));
            switch (tv->index) {
            case Vif0:
                reduce = feedVif0Pipe(tv).first; break;
            }

            highCycles -= std::max(reduce, static_cast<u32>(1));
            if (tv->isScratch)
                highCycles -= 0xc;

            if (!hasOwner)
                highCycles = 0;
        }
        highCycles = 0;
    }
    os::vec DmaController::performRead(u32 address) {
        os::vec request{};

        switch (address) {
        case 0x1000e010:
            request = intStatus;
            break;
        case 0x1000e020:
            request = *priorityCtrl;
            break;
        }
        return request;
    }
    void DmaController::issueADmacRequest(DirectChannels channel) {
        channels[channel].request = true;
    }

    std::pair<u32, u8> DmaController::feedVif0Pipe(RawReference<DmaChannel> vifc) {
        u32 transferred{};
        auto [isnTag, count] = pipeQuad2Transfer(vifc);
        if (!isnTag) {
            u32 remainFifoSpace{hw.vif0->getFifoFreeSpace()};
            switch (remainFifoSpace) {
            case 8:
                hw.vif0->transferDmaData({});
                hw.vif0->transferDmaData({});
                hw.vif0->transferDmaData({});
                hw.vif0->transferDmaData({});
                transferred += 4;
            case 4:
                hw.vif0->transferDmaData({});
                hw.vif0->transferDmaData({});
                transferred += 2;
            case 2:
                hw.vif0->transferDmaData({});
                hw.vif0->transferDmaData({});
                transferred += 2;
            }

            while (remainFifoSpace-- > 0 &&
                transferred < count) {
                hw.vif0->transferDmaData({}, true);
                transferred++;
            }
        }
        return {transferred, 0};
    }

    std::pair<bool, u32> DmaController::pipeQuad2Transfer(RawReference<DmaChannel> ch) {
        constexpr u8 qwcPerRequest{8};
        if (!ch->qwc)
            return std::make_pair(false, 0);
        u32 maxQwc{qwcPerRequest - (ch->adr >> 0x4) & 0x7};
        if (maxQwc >= std::numeric_limits<u16>::max())
            ;

        u32 toTransfer{std::min(ch->qwc, static_cast<u16>(maxQwc))};
        return {true, toTransfer};
    }
    void DmaController::disableChannel(DirectChannels channel, bool disableRequest) {
        bool isDisable{!disableRequest};
        u32 index{static_cast<u32>(channel)};
        auto& tv{channels.at(index)};

        if (disableRequest) {
            isDisable = tv.request;
            tv.index = false;
            findNextChannel();
        }
        if (!isDisable)
            return;

        if (hasOwner.id == index) {
            hasOwner.unselect();
            return;
        }
        std::list<DmaChannel>::iterator del{getStagedChannel(static_cast<DmaChannelId>(index))};
        for (; del != std::end(queued); ) {
            del = getStagedChannel(static_cast<DmaChannelId>(index));
            queued.erase(del);

            // Only one stream to remove? I don't understand why yet
            break;
        }
    }
    void DmaController::findNextChannel() {
        if (hasOwner.locked) {
            queued.push_back(channels[hasOwner.id]);
            hasOwner.unselect();
        }
        std::list<DmaChannel>::value_type channel;

        if (queued.size() == 0)
            ;

        channel = queued.front();
        hasOwner.select(channel.index);

        queued.pop_front();
    }
}
