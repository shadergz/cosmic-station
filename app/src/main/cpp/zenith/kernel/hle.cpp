#include <kernel/hle.h>
#include <eeiv/ee_assembler.h>

namespace zenith::kernel {
    static inline void writeAsmAt(u32& vectorIndex, std::span<u32>& block, u32 encoded) {
        block[vectorIndex += 4] = encoded;
    }

    u32 BiosHLE::prodAsmIntHandler(std::span<u32>& block) {
        const u32 codeBlockAt{0x00000200};
        u32 vRangeAdd{};

        // Configuring the $sp through a privileged register ($k0: Kernel Register #0)
        // lui $at, 0x0003
        writeAsmAt(vRangeAdd, block, eeiv::EECoreAssembler::lui(eeiv::MipsRegsHw::$at, 0x00030000));
        // ori $k0, $at, 0x0000
        writeAsmAt(vRangeAdd, block, eeiv::EECoreAssembler::ori(eeiv::MipsRegsHw::$k0, eeiv::MipsRegsHw::$at, 0));
        // addiu $k0, $k0, -0x230
        writeAsmAt(vRangeAdd, block, eeiv::EECoreAssembler::addiu(eeiv::MipsRegsHw::$k0, eeiv::MipsRegsHw::$k0, -0x230));

        // Saving all the registers
        for (u8 reg{}; reg <= eeiv::MipsRegsHw::$ra; reg++) {
            writeAsmAt(vRangeAdd, block, eeiv::EECoreAssembler::sq(static_cast<eeiv::MipsRegsHw>(reg),
                eeiv::MipsRegsHw::$k0, reg * 16));
        }

        // Storing the value of the kernel register ($k0) in the stack register ($sp)
        // add $sp, $k0, $zero
        writeAsmAt(vRangeAdd, block, eeiv::EECoreAssembler::add(eeiv::MipsRegsHw::$sp,
            eeiv::MipsRegsHw::$k0, eeiv::MipsRegsHw::$zero));

        // Loading INTC_STAT from 0x1000f000 into register $t1 and INTC_MASK from 0x1000f010 into $t2."
        writeAsmAt(vRangeAdd, block, eeiv::EECoreAssembler::lui(
            eeiv::MipsRegsHw::$at, 0x10000000));
        writeAsmAt(vRangeAdd, block, eeiv::EECoreAssembler::ori(
            eeiv::MipsRegsHw::$t0, eeiv::MipsRegsHw::$at, 0xf000));
        writeAsmAt(vRangeAdd, block, eeiv::EECoreAssembler::lw(
            eeiv::MipsRegsHw::$t1, eeiv::MipsRegsHw::$t0, 0));
        writeAsmAt(vRangeAdd, block, eeiv::EECoreAssembler::lw(
            eeiv::MipsRegsHw::$t2, eeiv::MipsRegsHw::$t0, 0x10));
        // Performing a bitwise AND operation between the two masks and storing the result back in the $t1 register
        writeAsmAt(vRangeAdd, block, eeiv::EECoreAssembler::_and(
            eeiv::MipsRegsHw::$t2, eeiv::MipsRegsHw::$t2, eeiv::MipsRegsHw::$t1));

        return codeBlockAt;
    }
}
