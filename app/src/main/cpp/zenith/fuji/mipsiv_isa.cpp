#include <fuji/mipsiv_interpreter.h>
#include <eeiv/ee_engine.h>
#include <eeiv/ee_assembler.h>

namespace zenith::fuji {
    template <typename T>
    T reg_cast(void* any) {
        return bit_cast<T>(any);
    }

    IvFuji3Impl(addi) {
        *gprDest = (sfet & 0xffff) + *gprSrc;
    }
    IvFuji3Impl(slti) {
        *gprDest = *gprSrc < (sfet & 0xffff);
    }
    IvFuji3Impl(sw) {
        // The 16-bit signed offset is added to the contents of GPR base to form the effective address
        u32 stAddr{*gprDest + (sfet & 0xffff)};
        mainMips.directWrite(stAddr, *gprSrc);
    }
    // if (cond < {0, null}) ...
    IvFuji3Impl(bltzal) {
        // With the 18-bit signed instruction offset, the conditional branch range is ± 128 KBytes
        i32 jump{static_cast<i32>((sfet & 0xffff) << 2)};
        // GPR[31] ← PC + 8
        *(mainMips.GprAt<u32*>(eeiv::$ra)) = *mainMips.lastPC + 8;
        mainMips.branchByCondition(*gprDest < 0, jump);
    }

#define LoadBy(dest, base, offset, from, to)\
    *bit_cast<to*>(dest) = static_cast<to>(mainMips.tableRead<from>(base + offset))

    IvFuji3Impl(lb) {
        LoadBy(gprSrc, *gprDest, (sfet & 0xffff), u8, i32);
    }
    IvFuji3Impl(lh) {
        LoadBy(gprSrc, *gprDest, (sfet & 0xffff), u16, i32);
    }
    IvFuji3Impl(lw) {
        LoadBy(gprSrc, *gprDest, (sfet & 0xffff), u32, i32);
    }
    IvFuji3Impl(lbu) {
        LoadBy(gprSrc, *gprDest, (sfet & 0xffff), u32, u32);
    }
    IvFuji3Impl(lhu) {
        LoadBy(gprSrc, *gprDest, (sfet & 0xffff), u16, u32);
    }
    IvFuji3Impl(lwu) {
        LoadBy(gprSrc, *gprDest, (sfet & 0xffff), u32, u32);
    }

    IvFuji3Impl(ld) {
        LoadBy(gprSrc, *gprDest, (sfet & 0xffff), u64, u64);
    }
    IvFuji3Impl(sd) {
        mainMips.directWrite((sfet & 0xffff) + *gprDest, *reg_cast<u64*>(gprSrc));
    }
#undef LoadBy
    IvFuji3Impl(cache) {
        const u32 as{*gprDest + (sfet & 0xffff)};
        switch (*gprSrc) {
        case 0x07:
            mainMips.cop0.invIndexed(as);
            break;
        }
    }
    IvFuji3Impl(nop) {}

    IvFujiSpecialImpl(ivXor) {
        *gprDest = *gprSrc ^ *gprExt;
    }
    IvFujiSpecialImpl(slt) {
        *gprDest = *gprSrc < *gprExt;
    }

    IvFuji3Impl(iBreak) {
        mainMips.handleException(1, 0x80000180, 0x9);
    }
}
