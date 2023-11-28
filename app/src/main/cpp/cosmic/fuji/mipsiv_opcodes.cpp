// SPDX-short-identifier: MIT, Version N/A
// This file is protected by the MIT license (please refer to LICENSE.md before making any changes, copying, or redistributing this software)
#include <common/global.h>
#include <fuji/mipsiv_interpreter.h>
#include <eeiv/ee_engine.h>
#define TranslateRegisters 1
namespace cosmic::fuji {
    using namespace eeiv;
    std::function<void(InvokeOpInfo&)> MipsIVInterpreter::decMipsIvS(u32 opcode, InvokeOpInfo& decode) {
        switch (opcode & 0x3f) {
        case SpecialSll: return [this](InvokeOpInfo& info) { sll(info.ops); };
        case SpecialSrl: return [this](InvokeOpInfo& info) { srl(info.ops); };
        case SpecialSra: return [this](InvokeOpInfo& info) { sra(info.ops); };
        case SpecialSllv: return [this](InvokeOpInfo& info) { sllv(info.ops); };
        case SpecialSrlv: return [this](InvokeOpInfo& info) { srlv(info.ops); };
        case SpecialSrav: return [this](InvokeOpInfo& info) { srav(info.ops); };
        case SpecialMovZ: return [this](InvokeOpInfo& info) { movz(info.ops); };
        case SpecialMovN: return [this](InvokeOpInfo& info) { movn(info.ops); };
        case SpecialSyscall: return [this](InvokeOpInfo& info) { ivSyscall(info.ops); };
        case SpecialBreak: return [this](InvokeOpInfo& info) { ivBreak(info.ops); };
        case SpecialMult: return [this](InvokeOpInfo& info) { mult(info.ops); };
        case SpecialMultu: return [this](InvokeOpInfo& info) { multu(info.ops); };
        case SpecialDiv: return [this](InvokeOpInfo& info) { div(info.ops); };
        case SpecialDivu: return [this](InvokeOpInfo& info) { divu(info.ops); };
        case SpecialAdd: return [this](InvokeOpInfo& info) { add(info.ops); };
        case SpecialAddu: return [this](InvokeOpInfo& info) { addu(info.ops); };
        case SpecialSub: return [this](InvokeOpInfo& info) { sub(info.ops); };
        case SpecialSubu: return [this](InvokeOpInfo& info) { subu(info.ops); };
        case SpecialDAdd: return [this](InvokeOpInfo& info) { dadd(info.ops); };
        case SpecialDAddu: return [this](InvokeOpInfo& info) { daddu(info.ops); };
        case SpecialDSub: return [this](InvokeOpInfo& info) { dsub(info.ops); };
        case SpecialDSubu: return [this](InvokeOpInfo& info) { dsubu(info.ops); };
        case SpecialXor: return [this](InvokeOpInfo& info) { ivXor(info.ops); };
        case SpecialSlt: return [this](InvokeOpInfo& info) { slt(info.ops); };
        }
        return {};
    }
    std::function<void(InvokeOpInfo&)> MipsIVInterpreter::decMipsIvRegImm(u32 opcode, InvokeOpInfo& decode) {
        u32 opImm{opcode >> 16 & 0x1f};
        switch (opImm) {
        case RegImmBltzal: return [this](InvokeOpInfo& info) { bltzal(info.ops); };
        }
        return {};
    }
    std::function<void(InvokeOpInfo&)> MipsIVInterpreter::decMipsIvCop0(u32 opcode, InvokeOpInfo& decode) {
        decode.pipe = OutOfOrder::Cop0;
        u8 cop{static_cast<u8>((opcode >> 26) & 0x3)};
        u32 op{(opcode >> 21) & 0x1f};
        if (cop == 2 && op > 0x10) {
        } else {
            switch (op | (cop * 0x100)) {
            case Cop0Mfc: return [this](InvokeOpInfo& info) { c0mfc(info.ops); };
            case Cop0Mtc: return [this](InvokeOpInfo& info) { c0mtc(info.ops); };
            case Cop0Bc0: return [this](InvokeOpInfo& info) { copbc0tf(info.ops); };
            case CopOp2Opcodes:
                u8 op2{static_cast<u8>(opcode & 0x3f)};
                switch (op2) {
                case CopOp2Tlbr: return [this](InvokeOpInfo& info) { tlbr(info.ops); };
                case CopOp2Eret:
                    decode.pipe = OutOfOrder::Eret;
                    return [this](InvokeOpInfo& info) { eret(info.ops); };
                case CopOp2Ei: return [this](InvokeOpInfo& info) { ei(info.ops); };
                case CopOp2Di: return [this](InvokeOpInfo& info) { di(info.ops); };
                }
            }
        }
        return {};
    }

    [[maybe_unused]] static std::array<const char*, 3> translatedGPRs{"Unk", "Unk", "Unk"};
    InvokeOpInfo MipsIVInterpreter::decMipsBlackBox(u32 opcode) {
        InvokeOpInfo decode{};
        std::array<u8, 3> operands{};
        for (u8 opi{}; opi < 3; opi++) {
            operands[opi] = (opcode >> (11 + opi * 5)) & 0x1f;
#if TranslateRegisters
            translatedGPRs[opi] = gprsId[operands.at(opi)];
#endif
        }
#if TranslateRegisters
        userLog->debug("(Mips FET) Opcode # {} PC # {} Decoded # 11, 16, 21: {}",
            opcode, *mainMips.eePC, fmt::join(translatedGPRs, " - "));
#endif
        decode.ops = Operands(opcode, operands);

        switch (opcode >> 26) {
        case SpecialOpcodes: decode.execute = decMipsIvS(opcode, decode); break;
        case RegImmOpcodes: decode.execute = decMipsIvRegImm(opcode, decode); break;
        case Addi: decode.execute = [this](InvokeOpInfo& info) { addi(info.ops); }; break;
        case EeSlti: decode.execute = [this](InvokeOpInfo& info) { slti(info.ops); }; break;
        case CopOpcodes: decode.execute = decMipsIvCop0(opcode, decode); break;
        case Lb: decode.execute = [this](InvokeOpInfo& info) { lb(info.ops); }; break;
        case Lh: decode.execute = [this](InvokeOpInfo& info) { lh(info.ops); }; break;
        case Lw: decode.execute = [this](InvokeOpInfo& info) { lw(info.ops); }; break;
        case Lbu: decode.execute = [this](InvokeOpInfo& info) { lbu(info.ops); }; break;
        case Lhu: decode.execute = [this](InvokeOpInfo& info) { lhu(info.ops); }; break;
        case Lwu: decode.execute = [this](InvokeOpInfo& info) { lwu(info.ops); }; break;
        case Cache: decode.execute = [this](InvokeOpInfo& info) { cache(info.ops); }; break;
        case Nop: decode.execute = [this](InvokeOpInfo& info) { nop(info.ops); }; break;
        case Ld: decode.execute = [this](InvokeOpInfo& info) { ld(info.ops); }; break;
        case Sw: decode.execute = [this](InvokeOpInfo& info) { sw(info.ops); }; break;
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