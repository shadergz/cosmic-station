// SPDX-short-identifier: MIT, Version N/A
// This file is protected by the MIT license (please refer to LICENSE.md before making any changes, copying, or redistributing this software)
#include <common/global.h>
#include <fuji/mipsiv_interpreter.h>
#include <eeiv/ee_engine.h>

#define SWCached(op)\
    decode.execute = [this](InvokeOpInfo& info) {\
        op(info.ops);\
    };\
    break

namespace cosmic::fuji {
    u32 MipsIVInterpreter::decMipsIvS(u32 opcode, InvokeOpInfo& decode) {
        switch (opcode & 0x3f) {
        case EeSpecialSyscall: SWCached(syscall);
        case SpecialBreak:     SWCached(iBreak);
        case EeSpecialXor:     SWCached(ivXor);
        case SpecialSlt:       SWCached(slt);
        }
        return opcode & 0x3f;
    }
    u32 MipsIVInterpreter::decMipsIvRegImm(u32 opcode, InvokeOpInfo& decode) {
        u32 opImm{opcode >> 16 & 0x1f};
        switch (opImm) {
        case RegImmBltzal:
            SWCached(bltzal);
        }
        return opImm;
    }
    u32 MipsIVInterpreter::decMipsIvCop0(u32 opcode, InvokeOpInfo& decode) {
        decode.pipe = OutOfOrder::Cop0;
        u8 cop{static_cast<u8>((opcode >> 26) & 0x3)};
        u32 op{(opcode >> 21) & 0x1f};
        if (cop == 2 && op > 0x10) {

        } else {
            switch (op | (cop * 0x100)) {
            case Cop0Mfc: SWCached(c0mfc);
            case Cop0Mtc: SWCached(c0mtc);
            case Cop0Bc0: SWCached(copbc0tf);
            case CopOp2:
                u8 op2{static_cast<u8>(opcode & 0x3f)};
                switch (op2) {
                case CopOp2Tlbr: SWCached(tlbr);
                case CopOp2Eret:
                    decode.pipe = OutOfOrder::Eret;
                    SWCached(eret);
                case CopOp2Ei:   SWCached(ei);
                case CopOp2Di:   SWCached(di);
                }
            }
        }
        return op;
    }
    static const std::array<const char*, 32> gprsId{
        "zero",
        "at", "v0", "v1", "a0", "a1", "a2", "a3",
        "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
        "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
        "t8", "t9",
        "k0", "k1",
        "gp", "sp", "fp", "ra"
    };
#define TranslateRegisters 0
    InvokeOpInfo MipsIVInterpreter::decMipsBlackBox(u32 opcode) {
        InvokeOpInfo decode{};
        std::array<u8, 3> operands{};
        operands[0] = opcode >> 11 & 0x1f;
        operands[1] = opcode >> 16 & 0x1f;
        operands[2] = opcode >> 21 & 0x1f;

#if TranslateRegisters
        static std::array<const char*, 3> translatedGPRs{"Unk", "Unk", "Unk"};
        translatedGPRs[0] = gprsId[operands.at(0)];
        translatedGPRs[1] = gprsId[operands.at(1)];
        translatedGPRs[2] = gprsId[operands.at(2)];

        userLog->debug("(Mips FET) Opcode # {} PC # {} Decoded # 11, 16, 21: {}",
            opcode, *mainMips.eePC, fmt::join(translatedGPRs, " - "));
#endif

        decode.ops = Operands(opcode, operands);
        switch (opcode >> 26) {
        case SpecialOpcodes:
            decMipsIvS(opcode, decode);
            break;
        case RegImmOpcodes:
            decMipsIvRegImm(opcode, decode);
            break;
        case Addi:   SWCached(addi);
        case EeSlti: SWCached(slti);
        case CopOpcodes:
            decMipsIvCop0(opcode, decode);
            break;
        case Lb:    SWCached(lb);
        case Lh:    SWCached(lh);
        case Lw:    SWCached(lw);
        case Lbu:   SWCached(lbu);
        case Lhu:   SWCached(lhu);
        case Lwu:   SWCached(lwu);
        case Cache: SWCached(cache);
        case Nop:   SWCached(nop);
        case Ld:    SWCached(ld);
        case Sw:    SWCached(sw);
        }
        return decode;
#undef SWQualified
    }
    u32 MipsIVInterpreter::fetchPcInst() {
        if (*mainMips.eePC & 4095)
            ;
        u32 save{mainMips.cyclesToWaste};
        u32 opcode{mainMips.fetchByPC()};

        mainMips.cyclesToWaste = save;
        return opcode;
    }
}