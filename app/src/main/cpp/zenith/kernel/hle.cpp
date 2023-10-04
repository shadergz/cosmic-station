#include <kernel/hle.h>
#include <eeiv/ee_assembler.h>

namespace zenith::kernel {
    static inline void emitCodeAt(u32& vectorIndex, std::span<u32>& block, u32 encoded) {
        block[vectorIndex++] = encoded;
    }

    static void ldMasksToT2(u32& range, std::span<u32>& block) {
        emitCodeAt(range, block, eeiv::EECoreAssembler::lui(eeiv::$at, 0x10000000));
        emitCodeAt(range, block, eeiv::EECoreAssembler::ori(eeiv::$t0, eeiv::$at, 0xf000));
        emitCodeAt(range, block, eeiv::EECoreAssembler::lw(eeiv::$t1, eeiv::$t0, 0));
        emitCodeAt(range, block, eeiv::EECoreAssembler::lw(eeiv::$t2, eeiv::$t0, 0x10));
        // Performing a bitwise AND operation between the two masks and storing the result back in the $t1 register
        emitCodeAt(range, block, eeiv::EECoreAssembler::_and(eeiv::$t2, eeiv::$t2, eeiv::$t1));
    }

    static void genLdOrStRegsFromK0(u32& range, std::span<u32>& block, bool save) {
        // This will make both sq and lq no longer constexpr functions
        std::function<u32(const eeiv::MipsRegsHw, const eeiv::MipsRegsHw, i16)> generator{};
        if (save)
            generator = eeiv::EECoreAssembler::sq;
        else
            generator = eeiv::EECoreAssembler::lq;

        for (u8 reg{}; reg <= eeiv::$ra; reg++) {
            emitCodeAt(range, block, generator(static_cast<eeiv::MipsRegsHw>(reg),
                eeiv::$k0, reg * 16));
        }
    }

    static void ldIntCAndJump(u32& range, std::span<u32>& block) {
        emitCodeAt(range, block, eeiv::EECoreAssembler::lui(eeiv::$at, 0x80000000));
        emitCodeAt(range, block, eeiv::EECoreAssembler::ori(eeiv::$t0, eeiv::$at, 0xa000));
        emitCodeAt(range, block, eeiv::EECoreAssembler::lw(eeiv::$t1, eeiv::$t0, 0));
        emitCodeAt(range, block, eeiv::EECoreAssembler::jalr(eeiv::$t1));
        emitCodeAt(range, block, 0);
    }

    void BiosHLE::resetBIOS() {
        const auto installAt{prodAsmIntHandler(intCodeASM)};
        mips->writeArray(installAt, intCodeASM);
    }

    u32 BiosHLE::prodAsmIntHandler(std::span<u32> block) {
        const u32 codeBlockAt{0x00000200};
        u32 vRangeAdd{};

        // Configuring the $sp through a privileged register ($k0: Kernel Register #0)
        // lui $at, 0x0003
        emitCodeAt(vRangeAdd, block, eeiv::EECoreAssembler::lui(eeiv::$at, 0x00030000));
        // ori $k0, $at, 0x0000
        emitCodeAt(vRangeAdd, block, eeiv::EECoreAssembler::ori(eeiv::$k0, eeiv::$at, 0));
        // addiu $k0, $k0, -0x230
        emitCodeAt(vRangeAdd, block, eeiv::EECoreAssembler::addiu(eeiv::$k0, eeiv::$k0, -0x230));
        // Saving all the registers
        genLdOrStRegsFromK0(vRangeAdd, block, true);
        // Storing the value of the kernel register ($k0) in the stack register ($sp)
        // add $sp, $k0, $zero
        emitCodeAt(vRangeAdd, block, eeiv::EECoreAssembler::add(eeiv::$sp, eeiv::$k0, eeiv::$zero));

        // Loading INTC_STAT from 0x1000f000 into register $t1 and INTC_MASK from 0x1000f010 into $t2."
        ldMasksToT2(vRangeAdd, block);
        emitCodeAt(vRangeAdd, block, eeiv::EECoreAssembler::sw(eeiv::$t2, eeiv::$t0, 0));
        ldIntCAndJump(vRangeAdd, block);

        // Restoring location of saved registers
        emitCodeAt(vRangeAdd, block, eeiv::EECoreAssembler::add(eeiv::$k0, eeiv::$sp, eeiv::$zero));
        // Loading all the registers back
        genLdOrStRegsFromK0(vRangeAdd, block, false);

        // (Exception return): Return to the program
        emitCodeAt(vRangeAdd, block, eeiv::EECoreAssembler::eret());
        return codeBlockAt;
    }
}
