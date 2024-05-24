#pragma once

#include <common/types.h>
#include <os/neon_simd.h>
#include <vu/vu_info.h>
namespace cosmic::engine {
    class EeMipsCore;
}
namespace cosmic::gs {
    class GifBridge;
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
        os::vec faster{};
    };
    union alignas(2) VuIntReg {
        VuIntReg() {}
        VuIntReg(i32 halfInt) :
            sig(static_cast<i16>(halfInt)) {}
        i16 sig;
        u16 uns;
    };
    struct SpecialVuEvent {
        bool isStarted;
        i64 finishAfter;
    };
    struct VuStatus {
        bool isVuExecuting;
        SpecialVuEvent div;
        SpecialVuEvent efu;
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
    enum VuSpecialReg {
        I, Q, R, P
    };

    class VectorUnit {
    public:
        VectorUnit() = delete;
        VectorUnit(Ref<VectorUnit> vu2, VuWorkMemory vuWm);

        void resetVu();
        void softwareReset();

        void pulse(u32 cycles);
        // Some implementations add a cycle delay at the beginning of the program (I won't do it now -
        // I still don't know how to solve this problem)
        void startProgram(u32 addr);
        void stopProgram();

        void propagateUpdates();
        void updateDivEfuPipes();
        u32 getMemMask() const noexcept;
        void stallByXgKick();
        void startXgKick2Gif();
        void handleDataTransfer();

        alignas(512) std::array<VuReg, 32> VuGPRs;
        alignas(32) std::array<VuIntReg, 16> intsRegs;

        void establishVif(u16 conTops[2], Ref<gs::GifBridge> gif);
        // P register: Used by EFU to store the result - waitp could be used to stall the execution
        // while EFU doesn't finish the previous calculation
        u32 vuPc{};

        void ctc(u32 index, u32 value);
        u32 cfc(u32 index);

        VuStatus status;
        VuIntPipeline intPipeline;
        Ref<VectorUnit> paraVu;
        void pushIntPipe(u8 ir, u8 fir);
        void finishStallPipeTask(bool isDiv);

        u32 fetchByPc();
        VuWorkMemory vecRegion;
        [[clang::always_inline]] void setSpecialReg(VuSpecialReg reg, const u32 uns) {
            if (reg == VuSpecialReg::I)
                spI.uns = uns;
            else if (reg == VuSpecialReg::Q)
                spQ.uns = uns;
            else if (reg == VuSpecialReg::R)
                spR.uns = uns;
            else if (reg == VuSpecialReg::P)
                spP.uns = uns;
        }

    private:
        VuRegUnique spI, spQ, spR, spP;
        VuRegUnique
            cachedQ,
            cachedP;

        std::shared_ptr<engine::EeMipsCore> ee;
        void updateMacPipeline();
        void updateDeltaCycles(i64 add, bool incCount = false);
        u16 vuf;

        bool isVuBusy,
            isOnBranch{false};
        // Each pipeline is specialized in a certain domain
        u64 pipeStates[2];

        std::array<u8, 4> clipFlags;
        u8 cfIndex;
        std::array<u16, 4> macFlags;
        u16 nextFlagsPipe;
        u8 mfIndex;
        u16 gifAddr;
        u16 gifStallAddr;

        struct {
            // If a second XGKICK is executed in the middle of the first, the instruction after the
            // second XGKICK will stall until the PATH1 transfer is complete
            bool stallXgKick{};
            bool transferringGif{};
            u32 cycles;
        } path1;

        std::unique_ptr<VuMicroExecutor> exe;
        struct {
            i64 count;
            i64 runCycles;
            i64 trigger;
            bool isDirty;
        } clock;
        u16* vifTops[2];
        std::optional<Ref<gs::GifBridge>> vu1Gif;
    };
}
