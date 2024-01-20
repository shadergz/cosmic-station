#pragma once
#include <mio/mem_pipe.h>

#include <common/types.h>
namespace cosmic {
    namespace spu {
        class Spu2;
    }
}

namespace cosmic::iop {
    struct IopDmaDevices {
        std::shared_ptr<spu::Spu2> sound;
    };
    // Chan is used here, but we can also refer to it as Channels
    struct IopChanControl {
        bool isFrom2Device;
        u8 syncMode;
        bool isBusy;
    };
    struct IopChan {
        u32 addr;
        u32 wordCount;
        u32 size;
        u16 blockSize;
        u8 index;
        u32 cyclesDelay;

        IopChanControl status;
    };
    enum DmaChannels {
        IopSpu2 = 0x8
    };
    class IopDma {
        static constexpr u8 invalidChannel{0x77};
    public:
        IopDma(std::shared_ptr<mio::MemoryPipe>& pipe) : ram(pipe) {
        }
        void resetIoDma();
        void pulse(u32 cycles);
        void connectDevices(IopDmaDevices& devices) {
            spu2 = devices.sound;
        }
    private:
        std::array<IopChan, 16> channels;
        std::shared_ptr<mio::MemoryPipe> ram;
        std::shared_ptr<spu::Spu2> spu2;

        template <typename T>
        T ioDmaRead(u32 iopAddr) {
            return *PipeCraftPtr<T*>(ram, iopAddr, mio::IopDev);
        }
        template <typename T>
        void ioDmaWrite(u32 iopAddr, u32 value) {
            *PipeCraftPtr<T*>(ram, iopAddr, mio::IopDev) = static_cast<u32>(value);
        }

        struct ActiveChannel {
            u16 channel;
            operator bool() {
                return channel != invalidChannel;
            }
            void reset() {
                channel = invalidChannel;
            }
        } activeChannel;
        void pulseSpu2Chain();
    };
}
