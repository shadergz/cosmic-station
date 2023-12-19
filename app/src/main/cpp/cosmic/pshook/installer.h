#pragma once
#include <common/types.h>

extern "C" {
    struct PsxRegs {
        cosmic::u32* a0;
        cosmic::u32* v0;
        cosmic::u32* pc;
        cosmic::u32* ra;
    };
    struct PsxRegs r;
    void psxAbs();
}

namespace pshook {
    extern struct PsxRegs eeSavedCtx;
    extern struct PsxRegs iopSavedCtx;
}
