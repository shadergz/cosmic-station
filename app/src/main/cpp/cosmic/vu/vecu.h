#pragma once

#include <common/types.h>
#include <os/neon_simd.h>

namespace cosmic::engine {
    class EeMipsCore;
}
namespace cosmic::gs {
    class GifArk;
}
namespace cosmic::vu {
    class VuWorkMemory {
    public:
        VuWorkMemory() = default;
        template<typename T>
        VuWorkMemory(T vum[2]) {
            rw = vum[0];
            re = vum[1];
        }
        std::span<u8> rw;
        std::span<u8> re;
    };
    union alignas(16) VuRegUnique {
        f32 hd;
        u32 uns;
        std::array<u8, 16> pad;
    };
    union alignas(16) VuReg {
        union {
            float x, y, z, w;
            f32 floats[4];
        };
        u32 uns[4];
        i32 sig[4];
        os::vec128 faster{};
    };
    union alignas(2) VuIntReg {
        VuIntReg() {}
        VuIntReg(i32 halfInt) :
            sig(static_cast<i16>(halfInt)) {}
        i16 sig;
        u16 uns;
    };
    struct VuStatus {
        bool isVuExecuting;
        bool isStartedDivEvent;
    };

    class VuIntPipeline {
    public:
        VuIntPipeline();

        struct PipeEntry {
            u8 affectedIr;
            VuIntReg originalValue;
            bool rw;

            void clearEntry() {
                affectedIr = 0;
                originalValue = {};
                rw = false;
            }
        };
        std::array<PipeEntry, 5> pipeline;
        u8 pipeCurrent;

        void pushInt(u8 ir, VuIntReg old, bool rw);
        void update();
        void flush();
    };

    class VectorUnit {
    public:
        VectorUnit() = delete;
        VectorUnit(VuWorkMemory vuWm);

        void resetVu();
        void softwareReset();

        void pulse(u32 cycles);
        alignas(512) VuReg VuGPRs[32];
        alignas(32) VuIntReg intsRegs[16];

        void establishVif(u16* conTops, RawReference<gs::GifArk> gif);
        // P register: Used by EFU to store the result; waitp could be used to stall the execution
        // while EFU doesn't finish the previous calculation
        VuRegUnique spI, spQ, spR, spP;
        void ctc(u32 index, u32 value);
        u32 cfc(u32 index);

        VuStatus status;
        VuIntPipeline intPipeline;
        void pushIntPipe(u8 ir, u8 fir);
    private:
        std::shared_ptr<engine::EeMipsCore> ee;
        void updateMacPipeline();
        void updateClock(i64 add);
        u16 vuf;

        bool isVuBusy, isOnBranch{false};
        // Each pipeline is specialized in a certain domain
        u64 pipeStates[2];

        std::array<u8, 4> clipFlags;
        u8 cfIndex;
        std::array<u16, 4> macFlags;
        u16 nextFlagsPipe;
        u8 mfIndex;

        u32 vuPc{};
        struct {
            i64 count;
            i64 runCycles;
            i64 trigger;
            bool isDirty;
        } clock;
        u16* vifTops[2];
        std::optional<RawReference<gs::GifArk>> vu1Gif;
        VuWorkMemory vecRegion;
    };
}
