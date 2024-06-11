#pragma once
#include <array>
#include <list>

#include <common/types.h>
#include <os/neon_simd.h>
#include <mio/blocks.h>
namespace cosmic {
    namespace vu {
        class VifMalice;
    }
    namespace ee {
        class EeMipsCore;
    }
}

namespace cosmic::mio {
    class MemoryPipe;

    struct HardWithDmaCap {
    public:
        HardWithDmaCap() {}
        Ref<vu::VifMalice>
            vif0,
            vif1;
        std::shared_ptr<ee::EeMipsCore> ee;
    };
    // D_STAT - DMAC interrupt status
    struct InterruptStatus {
        std::array<bool, 0xf> channelStat;
        std::array<bool, 0xf> channelMask;
    };
    // D_SQWC - DMAC skip quadword
    struct SkipQuadword {
        u8 skipQwc;
        u8 transferQwc;
    };

    enum DirectChannels {
        Vif0, Vif1,
        Gif,
        IpuFrom, IpuTo,
        Sif0, Sif1, Sif2,
        SprFrom,
        SprTo,

        DmaStall = 0xd,
        FifoEmpty
    };
    using DmaChannelId = u8;
    struct DmaChannel {
        alignas(4) u32 qwc;
        // This will result in having both Dn_MADR and Dn_TADR,
        // including Dn_ASRX and Dn_SADR in a single location
        struct {
            u32 adr;
            u32 tagAdr;
            // SPR_FROM, SPR_TO
            bool isScratch;
            bool isChan;
            u8 tagType;
        };
        struct {
            bool request{false};
            bool started;

            bool hasStallDrain;
            bool hasDmaStalled;
        };

        DmaChannelId index;
    };
    union DmaRegister {
        u32 address;
        u32 value;
        bool writeBack{false};

        auto operator=(u32 vReg) {
            value = vReg;
            writeBack = true;
        }
        auto operator*() {
            return value;
        }
    };
    // D_CTRL - DMAC control
    struct DmaStatus {
        bool isDmaEnabled;
        bool isCycleStealing;

        u8 mFiFoChannel;
        u8 drainChannel;
        u8 stallSrcChannel,
            stallDestChannel;
        // When isCycleStealing is true, releaseCyclePeriod will determine how many machine
        // cycles the EE can freely use the BUS without congestion
        u8 releaseCyclePeriod;
    };
    class OwnerChannel {
    public:
        OwnerChannel() : id(), locked() {
        }
        operator bool() const {
            return locked;
        }
        inline void select(u8 cid) {
            if (locked)
                return;
            id = cid;
            locked = true;
        }
        inline u8 unselect() {
            bool isl{locked};
            locked = false;
            if (isl)
                return id;
            return {};
        }
        inline auto getId() const {
            return id;
        }
    private:
        DmaChannelId id;
        bool locked;
    };

    // https://fobes.dev/ee/2024/02/02/ps2-dmac-basics.html
    class DmaController {
    public:
        DmaController();

        void resetMa();
        void pulse(u32 cycles);
        os::vec performRead(u32 address);
        Ref<u32> dmaVirtSolver(u32 address);

        Ref<u128> dmaAddrSolver(u32 address, bool isScr, bool isVu);
        os::vec dmacRead(u32& address);
        void dmacWrite(u32 address, const os::vec& val);

        void issueADmacRequest(DirectChannels channel);
        void connectDevices(HardWithDmaCap& devices);

        std::shared_ptr<GlobalMemory> mapped;
        DmaStatus status;

        // Changes the internal behavior of the DMAC, causing interruptions
        union InterruptReg {
            struct {
                u8 control: 6;
                u8 unused: 7;
                bool busError: 1;
                u8 chIntMask: 6;
                u8 chInt: 6;
                u8 iopMasterInt: 1;
            };
            u32 dicr;
        } ir; // DICR
        // Clean the DMA request from the channel (Doesn't clear the channel itself, only the request flag)
        void disableChannel(DirectChannels channel, bool disableRequest = false);
        void advanceSrcDma(DmaChannel& chan);
        void advanceSrcDma(DirectChannels id) {
            if (static_cast<u32>(id) > channels.size()) {
            }
            auto& chan{channels[id]};
            advanceSrcDma(chan);
        }
        void raiseInt1();

    private:
        std::list<DmaChannelId> queued;
        using ChannelIterator = std::list<DmaChannelId>::iterator;

        u32 intStatus;
        OwnerChannel hasOwner;
        i64 highCycles;

        std::pair<u32, u8> feedVif0Pipe(Ref<DmaChannel> vifc);
        std::pair<bool, u32> pipeQuad2Transfer(Ref<DmaChannel> ch);
        void checkStallOrActivateLater(DirectChannels channel);

        void switchChannel();
        void findNextChannel();
        union {
            std::array<DmaChannel, 0x9> channels;
        };
        std::function<ChannelIterator(DmaChannelId)> getStagedChannel;

        DmaRegister priorityCtrl{0x1f8010f0}; // PCR
        InterruptStatus intStat;
        SkipQuadword skip;

        u32 stallAddress; // STADR
        std::shared_ptr<MemoryPipe> pipe;

        struct {
            Ref<vu::VifMalice> vif1, vif0;
            std::shared_ptr<ee::EeMipsCore> core;
        } hw;
    };
}
