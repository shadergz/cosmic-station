#pragma once
#include <mutex>
#include <common/types.h>

namespace cosmic::iop {
    class IoMipsCore;
}
#define NO_MANGLE extern "C"
namespace cosmic::pshook {
    NO_MANGLE {
        union PsxR { u32 i; i32 s; };
        struct PsxRegs {
            PsxR* a0;
            PsxR* v0;
            PsxR* ra;
            PsxR* pc;
        };
        extern struct PsxRegs r;
        void psxAbs();
    }

    extern struct PsxRegs eeSavedCtx;
    extern struct PsxRegs iopSavedCtx;

    class EstablishHook {
    public:
        static void hookIoPsx(u32 base, iop::IoMipsCore& psx);
    private:
        static std::mutex owner;
        static u8 isEe;
    };
}
