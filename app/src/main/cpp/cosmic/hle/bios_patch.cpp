#include <hle/bios_patch.h>
#include <ee/ee_assembler.h>

namespace cosmic::hle {
    void BiosPatcher::emit(u32 address) {
        mips->writeArr(address, intCodeAsm);
    }
    void BiosPatcher::andIntCStatToT2(u32& range) {
        intCodeAsm[range++] = ee::EeCoreAssembler::lui(ee::$at, 0x10000000);
        intCodeAsm[range++] = ee::EeCoreAssembler::ori(ee::$t0, ee::$at, 0xf000);
        intCodeAsm[range++] = ee::EeCoreAssembler::lw(ee::$t1, ee::$t0, 0);
        intCodeAsm[range++] = ee::EeCoreAssembler::lw(ee::$t2, ee::$t0, 0x10);
        // Performing a bitwise AND operation between the two masks and storing the result back in the $t1 register
        intCodeAsm[range++] = ee::EeCoreAssembler::ivAnd(ee::$t2, ee::$t2, ee::$t1);
    }

    void BiosPatcher::regsFromKernel0(u32& range, bool save) {
        // This will make both sq and lq no longer constexpr functions
        std::function<u32(const ee::MipsRegsHw, const ee::MipsRegsHw, i16)> generator{};
        if (save)
            generator = ee::EeCoreAssembler::sq;
        else
            generator = ee::EeCoreAssembler::lq;

        for (u8 reg{}; reg <= ee::$ra; reg++)
            intCodeAsm[range++] = generator(static_cast<ee::MipsRegsHw>(reg), ee::$k0, reg * 16);
    }

    void BiosPatcher::intCAndJump(u32& range) {
        intCodeAsm[range++] = ee::EeCoreAssembler::lui(ee::$at, 0x80000000);
        intCodeAsm[range++] = ee::EeCoreAssembler::ori(ee::$t0, ee::$at, 0xa000);
        intCodeAsm[range++] = ee::EeCoreAssembler::lw(ee::$t1, ee::$t0, 0);
        intCodeAsm[range++] = ee::EeCoreAssembler::jalr(ee::$t1);
        intCodeAsm[range++] = 0x0;
    }

    void BiosPatcher::resetBios() {
        const auto installAt{prodAsmIntHandler()};
        emit(installAt);
    }
    u32 BiosPatcher::prodAsmIntHandler() {
        const u32 codeBlockAt{0x00000200};
        u32 lane{};

        // Configuring the $sp through a privileged register ($k0: Kernel Register #0)
        // lui $at, 0x0003
        intCodeAsm[lane++] = ee::EeCoreAssembler::lui(ee::$at, 0x00030000);
        // ori $k0, $at, 0x0000
        intCodeAsm[lane++] = ee::EeCoreAssembler::ori(ee::$k0, ee::$at, 0);
        // addiu $k0, $k0, -0x230
        intCodeAsm[lane++] = ee::EeCoreAssembler::addiu(ee::$k0, ee::$k0, -0x230);
        // Saving all the registers
        regsFromKernel0(lane, true);
        // Storing the value of the kernel register ($k0) in the stack register ($sp)
        // add $sp, $k0, $zero
        intCodeAsm[lane++] = ee::EeCoreAssembler::add(ee::$sp, ee::$k0, ee::$zero);

        // Loading INTC_STAT from 0x1000f000 into register $t1 and INTC_MASK from 0x1000f010 into $t2."
        andIntCStatToT2(lane);
        intCodeAsm[lane++] = ee::EeCoreAssembler::sw(ee::$t2, ee::$t0, 0);
        intCAndJump(lane);

        // Restoring location of saved registers
        intCodeAsm[lane++] = ee::EeCoreAssembler::add(ee::$k0, ee::$sp, ee::$zero);
        // Loading all the registers back
        regsFromKernel0(lane, false);

        // (Exception return): Return to the program
        intCodeAsm[lane++] = ee::EeCoreAssembler::eret();
        return codeBlockAt;
    }
}
