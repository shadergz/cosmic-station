// SPDX-short-identifier: MIT, Version N/A
// This file is protected by the MIT license (please refer to LICENSE.md before making any changes, copying, or redistributing this software)
#include <common/global.h>
#include <creeper/ee/mipsiv_cached.h>
#include <engine/ee_core.h>
#define TRANSLATE_REGISTERS 0
namespace cosmic::creeper::ee {
    using namespace engine;
    InvokableCached MipsIvInterpreter::execSpecial(u32 opcode, InvokeOpInfo& decode) {
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
        case SpecialMult:
        case SpecialMultu: {
            decode.pipe = OutOfOrder::EffectivePipeline::Mac0;
            decode.extraCycles = Mul;
            if ((opcode & 0x3f) == SpecialMult)
                return [this](InvokeOpInfo& info) { mult(info.ops); };
            else
                return [this](InvokeOpInfo& info) { multu(info.ops); };
        }
        case SpecialDiv:
        case SpecialDivu: {
            decode.pipe = OutOfOrder::EffectivePipeline::Mac0;
            decode.extraCycles = Div;
            if ((opcode & 0x3f) == SpecialDiv)
                return [this](InvokeOpInfo& info) { div(info.ops); };
            else return [this](InvokeOpInfo& info) { divu(info.ops); };
        }
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
    InvokableCached MipsIvInterpreter::execRegimm(u32 opcode, InvokeOpInfo& decode) {
        u32 opImm{opcode >> 16 & 0x1f};
        switch (opImm) {
        case RegImmBltzal: return [this](InvokeOpInfo& info) { bltzal(info.ops); };
        }
        return {};
    }
    InvokableCached MipsIvInterpreter::execCop(u32 opcode, InvokeOpInfo& decode) {
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
                case CopOp2Eret: {
                    decode.pipe = OutOfOrder::Eret;
                    return [this](InvokeOpInfo& info) { eret(info.ops); };
                }
                case CopOp2Ei: return [this](InvokeOpInfo& info) { ei(info.ops); };
                case CopOp2Di: return [this](InvokeOpInfo& info) { di(info.ops); };
                }
            }
        }
        return {};
    }

    thread_local std::array<const char*, 3> translatedGprs{"Unk", "Unk", "Unk"};
    InvokeOpInfo MipsIvInterpreter::execBlackBox(u32 opcode) {
        InvokeOpInfo decode{};
        std::array<u8, 3> operands{};
        for (u8 opi{}; opi < 3; opi++) {
            operands[opi] = (opcode >> (11 + opi * 5)) & 0x1f;
#if TRANSLATE_REGISTERS
            translatedGprs[opi] = gprsNames[operands.at(opi)];
#endif
        }
#if TRANSLATE_REGISTERS
        user->debug("(Mips FETCH) Opcode # {} pc # {} decoded # 11, 16, 21: {}",
            opcode, *cpu->eePc, fmt::join(translatedGprs, " - "));
#endif
        decode.ops = Operands(opcode, operands);
        decode.execute = [](InvokeOpInfo& err) {
            std::array<std::string, 3> gprs{
                std::string{"$"} + gprsNames[err.ops.gprs[0]],
                std::string{"$"} + gprsNames[err.ops.gprs[1]],
                std::string{"$"} + gprsNames[err.ops.gprs[2]]
            };

            throw AppErr("Invalid or unrecognized opcode {:#x}, parameters: Unk {}", err.ops.inst,
                fmt::join(gprs, ", "));
        };

        switch (opcode >> 26) {
        case SpecialOpcodes: decode.execute = execSpecial(opcode, decode); break;
        case RegImmOpcodes: decode.execute = execRegimm(opcode, decode); break;
        case Addi: decode.execute = [this](InvokeOpInfo& info) { addi(info.ops); }; break;
        case Bne: decode.execute = [this](InvokeOpInfo& info) { bne(info.ops); }; break;
        case Slti: decode.execute = [this](InvokeOpInfo& info) { slti(info.ops); }; break;
        case CopOpcodes: decode.execute = execCop(opcode, decode); break;
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

    }
    u32 MipsIvInterpreter::fetchPcInst(u32 pc) {
        if (pc & 4095) {
            if (cpu->GPRs[26].words[0] == 0)
                if (cpu->GPRs[25].words[0] == 0)
                    if (cpu->GPRs[30].dw[0] + cpu->GPRs[31].dw[0] == 0) {
                    }
        }
        const u32 opcode{cpu->fetchByAddress(pc)};
        return opcode;
    }
}