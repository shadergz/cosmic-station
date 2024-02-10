#include <common/types.h>
#include <os/neon_simd.h>
#include <common/asm_c++out.h>
namespace cosmic::gs {
    extern "C" {
        i32 gSize;
        bool gResetDone;
        void* gBackPtr;
        void* gFrontPtr;

        std::array<u8, 16 * 16> gQueue;
    }

    [[gnu::naked]] void gifQueueReset() {
        PROLOGUE_ASM(16);
        __asm("eor v0.16b, v0.16b, v0.16b\n" // v0 = 0.0, 0.0 ...
            "mov x9, 0\n"
            "mov x0, 0\n");
        STR_LTO_A64(x9, gBackPtr, x0);
        ADR_LTO_A64(x10, gSize);
        __asm("eor x0, x0, x0\n"
            "str w0, [x10]\n"
            "mov w12, 0\n");
        ADR_LTO_A64(x10, gQueue);

        __asm("cleanUp:\n"
            "mov w9, w12\n"
            "lsl x9, x9, #4\n"
            "add x11, x10, x9\n"
            "st1 {v0.16b}, [x11]\n" // ((u128*)qQueue[w0]) = v0

            "add w12, w12, #1\n"
            "sub w11, w12, #16\n"
            "cbz w11, #8\n"
            "b cleanUp\n");

        ADR_LTO_A64(x10, gResetDone);
        __asm("mov w11, #1\n"
            "strb w11, [x10]\n");
        EPILOGUE_ASM(16);
        __asm("ret");
    }

    [[gnu::naked]] u8 gifQueueSize() {
        PROLOGUE_ASM(16);
        // We can pre-load the array values into the L2 cache since we'll be accessing it shortly
        __asm("mov x10, 0\n"
            "mov w11, 0\n");
        ADR_LTO_A64(x9, gQueue);
        __asm("loadIntoL2:\n"

            // ((u8*)gQueue)[w1 * 64]
            "mov x10, x11\n"
            "lsl x10, x10, #6\n"
            "add x12, x9, x10\n"
            "prfm pldl2keep, [x12]\n"
            "add w11, w11, #1\n"
            "sub w10, w11, #4\n"
            "cbz w10, #0x8\n"
            "b loadIntoL2\n");
        ADR_LTO_A64(x0, gSize);

        __asm("ldr w0, [x0]\n");
        EPILOGUE_ASM(16);
        __asm("ret");
    }
}