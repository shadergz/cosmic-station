#include <hle/bios_patch.h>
#include <eeiv/ee_assembler.h>

namespace zenith::hle {
    void BiosPatcher::emit(u32 address) {
        mips->writeArray(address, intCodeASM);
    }

    void BiosPatcher::andIntCStatToT2(u32& range) {
        intCodeASM[range++] = eeiv::EECoreAssembler::lui(eeiv::$at, 0x10000000);
        intCodeASM[range++] = eeiv::EECoreAssembler::ori(eeiv::$t0, eeiv::$at, 0xf000);
        intCodeASM[range++] = eeiv::EECoreAssembler::lw(eeiv::$t1, eeiv::$t0, 0);
        intCodeASM[range++] = eeiv::EECoreAssembler::lw(eeiv::$t2, eeiv::$t0, 0x10);
        // Performing a bitwise AND operation between the two masks and storing the result back in the $t1 register
        intCodeASM[range++] = eeiv::EECoreAssembler::ivAnd(eeiv::$t2, eeiv::$t2, eeiv::$t1);
    }

    void BiosPatcher::regsFromKernel0(u32& range, bool save) {
        // This will make both sq and lq no longer constexpr functions
        std::function<u32(const eeiv::MipsRegsHw, const eeiv::MipsRegsHw, i16)> generator{};
        if (save)
            generator = eeiv::EECoreAssembler::sq;
        else
            generator = eeiv::EECoreAssembler::lq;

        for (u8 reg{}; reg <= eeiv::$ra; reg++)
            intCodeASM[range++] = generator(static_cast<eeiv::MipsRegsHw>(reg), eeiv::$k0, reg * 16);
    }

    void BiosPatcher::intCAndJump(u32& range) {
        intCodeASM[range++] = eeiv::EECoreAssembler::lui(eeiv::$at, 0x80000000);
        intCodeASM[range++] = eeiv::EECoreAssembler::ori(eeiv::$t0, eeiv::$at, 0xa000);
        intCodeASM[range++] = eeiv::EECoreAssembler::lw(eeiv::$t1, eeiv::$t0, 0);
        intCodeASM[range++] = eeiv::EECoreAssembler::jalr(eeiv::$t1);
        intCodeASM[range++] = 0;
    }

    void BiosPatcher::resetBIOS() {
        const auto installAt{prodAsmIntHandler()};
        emit(installAt);
    }

    u32 BiosPatcher::prodAsmIntHandler() {
        const u32 codeBlockAt{0x00000200};
        u32 lane{};

        // Configuring the $sp through a privileged register ($k0: Kernel Register #0)
        // lui $at, 0x0003
        intCodeASM[lane++] = eeiv::EECoreAssembler::lui(eeiv::$at, 0x00030000);
        // ori $k0, $at, 0x0000
        intCodeASM[lane++] = eeiv::EECoreAssembler::ori(eeiv::$k0, eeiv::$at, 0);
        // addiu $k0, $k0, -0x230
        intCodeASM[lane++] = eeiv::EECoreAssembler::addiu(eeiv::$k0, eeiv::$k0, -0x230);
        // Saving all the registers
        regsFromKernel0(lane, true);
        // Storing the value of the kernel register ($k0) in the stack register ($sp)
        // add $sp, $k0, $zero
        intCodeASM[lane++] = eeiv::EECoreAssembler::add(eeiv::$sp, eeiv::$k0, eeiv::$zero);

        // Loading INTC_STAT from 0x1000f000 into register $t1 and INTC_MASK from 0x1000f010 into $t2."
        andIntCStatToT2(lane);
        intCodeASM[lane++] = eeiv::EECoreAssembler::sw(eeiv::$t2, eeiv::$t0, 0);
        intCAndJump(lane);

        // Restoring location of saved registers
        intCodeASM[lane++] = eeiv::EECoreAssembler::add(eeiv::$k0, eeiv::$sp, eeiv::$zero);
        // Loading all the registers back
        regsFromKernel0(lane, false);

        // (Exception return): Return to the program
        intCodeASM[lane++] = eeiv::EECoreAssembler::eret();
        return codeBlockAt;
    }
}
