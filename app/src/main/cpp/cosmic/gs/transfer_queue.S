.data
gSize: .int 0
gResetDone: .byte 0
.align 8
gBackPtr: .quad 0
gFrontPtr: .quad 0
.bss
.align 16
gQueue: .space 16 * 16

gifQueueReset:
    PROLOGUE_ASM(16);
    stp, x29, x30, [sp, #-16]
    eor v0.16b, v0.16b, v0.16b // v0 = 0.0, 0.0 ...
    mov x9, 0
    mov x0, 0
    str x9, [gBackPtr, x0]

    adr x10, gSize
    eor x0, x0, x0
    str w0, [x10]
    mov w12, 0
    adr x10, gQueue
cleanUp:
    mov w9, w12\n
    lsl x9, x9, #4
    add x11, x10, x9
    st1 {v0.16b}, [x11] // ((u128*)qQueue[w0]) = v0

    add w12, w12, #1
    sub w11, w12, #16
    cbz w11, #8
    b cleanUp

    adr x10, gResetDone
    mov w11, #1
    strb w11, [x10]
    ldp x29, x30, [sp], #16
    ret

gifQueueSize:
    stp, x29, x30, [sp, #-16]
    // We can pre-load the array values into the L2 cache since we'll be accessing it shortly
    mov x10, 0
    mov w11, 0
    adr x9, gQueue
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
    adr x0, gSize

    ldr w0, [x0]
    ldp x29, x30, [sp], #16
    ret
