.arch armv8-a
#include <common/asm_c++out.h>

#define gifQueueReset _ZN6cosmic2gs13gifQueueResetEv
#define gifQueueSize _ZN6cosmic2gs12gifQueueSizeEv

.global gifQueueReset, gifQueueSize
declFunc gifQueueReset
declFunc gifQueueSize

.text
// Macro adr_l and unconditional jumps are being used as a priority, due to the enabled LTO
gifQueueReset:
    prologue
    eor v0.16b, v0.16b, v0.16b // v0 = 0.0, 0.0 ...
    mov x9, 0
    mov x0, 0
    strA64 x9, gBackPtr, x0
    strA64 x9, gFrontPtr, x0
    eor x0, x0, x0
    adrA64 x10, gSize
    str w0, [x10]

    mov w12, 0
    adrA64 x10, gQueue
cleanUp:
    mov w9, w12
    lsl x9, x9, #4
    add x11, x10, x9
    st1 {v0.16b}, [x11] // ((u128*)qQueue[w0]) = v0

    add w12, w12, #1
    sub w11, w12, #16
    cbz w11, #8
    b cleanUp
    adrA64 x10, resetDone
    mov w11, #1
    strb w11, [x10]

    epilogue
    ret
gifQueueSize:
    prologue
    // We can pre-load the array values into the L2 cache since we'll be accessing it shortly
    mov x10, 0
    mov w11, 0
    adrA64 x9, gQueue
loadIntoL2:
    // ((u8*)gQueue)[w1 * 64]
    mov x10, x11
    lsl x10, x10, #6
    add x12, x9, x10

    prfm pldl2keep, [x12]

    add w11, w11, #1
    sub w10, w11, #4
    cbz w10, #0x8
    b loadIntoL2

    adrA64 x0, gSize
    ldr w0, [x0]
    epilogue
    ret

.data
gSize: .int 0
resetDone: .byte 0

.align 8
gBackPtr: .quad 0
gFrontPtr: .quad 0

.bss
.align 16
gQueue:
    .space 16 * 16
.end
