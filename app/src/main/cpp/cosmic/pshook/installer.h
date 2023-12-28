#pragma once
#include <mutex>
#include <common/types.h>

extern "C" {
    struct PsxRegs {
        cosmic::u32* a0;
        cosmic::u32* v0;
        cosmic::u32* ra;
        cosmic::u32* pc;
    };
    extern struct PsxRegs r;
    void psxAbs();
}

namespace cosmic::iop {
    class IoMipsCore;
}

namespace cosmic::pshook {
    extern struct PsxRegs eeSavedCtx;
    extern struct PsxRegs iopSavedCtx;

    class HookOwner {
    public:
        static void hookIoPsx(u32 base, iop::IoMipsCore& psx);
    private:
        static std::mutex owner;
        static u8 isEe;
    };
}
