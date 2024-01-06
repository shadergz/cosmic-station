.arch armv8-a

#include <common/asm_c++out.h>

.set gifQueueReset, _ZN6cosmic2gs13gifQueueResetEv
.set gifQueueSize, _ZN6cosmic2gs13gifQueueResetEv

.global gifQueueReset, gifQueueSize
declFunc gifQueueReset
declFunc gifQueueSize

.text
// Macro adr_l and unconditional jumps are being used as a priority, due to the enabled LTO

gifQueueReset:
    eor v0.16b, v0.16b, v0.16b
    mov x0, 0
    adr_l x1, gQueue
cleanUp:
    lsl x1, x0, #6
    st1 {v0.16b}, [x1] // ((u128*)qQueue[w0]) = v0

    add w0, w0, #1
    cmp w0, #16
    sub w3, w0, #16
    cbz w3, #4
    b cleanUp

    ret
gifQueueSize:
    // We can pre-load the array values into the L2 cache since we'll be accessing it shortly
    mov x1, 0
    adr_l x0, gQueue
loadIntoL2:
    // ((u8*)gQueue)[w1 * 64]
    lsl x0, x1, #6
    prfm pldl2keep, [x2]

    add w1, w1, #4
    sub w3, w1, #16
    cbz w3, #0x8
    b loadIntoL2

    ldr_l x0, qSize
    ret

.data
qSize: .int 0

.align 8
gBackPtr: .long 0
gFrontPtr: .long 0

.bss
.align 16

gQueue:
    .space 16 * 16
.end

