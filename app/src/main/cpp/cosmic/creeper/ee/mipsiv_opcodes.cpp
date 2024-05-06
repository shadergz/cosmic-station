// SPDX-short-identifier: MIT, Version N/A
// This file is protected by the MIT license (please refer to LICENSE.md before making any changes, copying, or redistributing this software)
#include <common/global.h>
#include <creeper/ee/mipsiv_cached.h>
#include <engine/ee_core.h>

namespace cosmic::creeper::ee {
    using namespace engine;

    EeMapSpecial MipsIvInterpreter::mapMipsSpecial{
        {SpecialSll, {sll, "sll"}},
        {SpecialSrl, {srl, "srl"}},
        {SpecialSra, {sra, "sra"}},
        {SpecialSllv, {sllv, "sllr"}},
        {SpecialSrlv, {srlv, "srlv"}},
        {SpecialSrav, {srav, "srav"}},
        {SpecialMovZ, {movz, "movz"}},
        {SpecialMovN, {movn, "movn"}},
        {SpecialSyscall, {ivSyscall, "syscall"}},
        {SpecialBreak, {ivBreak, "break"}},

        {SpecialMult, {mult, "mult"}},
        {SpecialMultu, {multu, "multu"}},

        {SpecialDiv, {div, "div"}},
        {SpecialDivu, {divu, "divu"}},

        {SpecialAdd, {add, "add"}},
        {SpecialAddu, {addu, "addu"}},
        {SpecialSub, {sub, "sub"}},
        {SpecialSubu, {subu, "subu"}},
        {SpecialDAdd, {dadd, "dadd"}},
        {SpecialDAddu, {daddu, "daadu"}},
        {SpecialDSub, {dsub, "dsub"}},
        {SpecialDSubu, {dsubu, "dsubu"}},
        {SpecialXor, {ivXor, "xor"}},
        {SpecialSlt, {slt, "slt"}}
    };

    void MipsIvInterpreter::decodeSpecial(u32 opcode, InvokeOpInfo& codes, EeInstructionSet& set) {
        switch (opcode & 0x3f) {
        case SpecialMult:
        case SpecialMultu:
            codes.pipe = OutOfOrder::EffectivePipeline::Mac0;
            codes.extraCycles = Mul;
            break;
        case SpecialDiv:
        case SpecialDivu:
            codes.pipe = OutOfOrder::EffectivePipeline::Mac0;
            codes.extraCycles = Div;
        }
        auto exclusiveOp{static_cast<MipsIvSpecial>(opcode & 0x3f)};
        if (mapMipsSpecial.contains(exclusiveOp)) {
            codes.execute = [exclusiveOp](InvokeOpInfo& info) {
                mapMipsSpecial[exclusiveOp].instHandler(info.ops);
            };
            set.r9OpcodeStr = mapMipsSpecial[exclusiveOp].instName;
        }
    }
    EeRegImm MipsIvInterpreter::mapMipsRegimm{
        {RegImmBltzal, {bltzal, "bltzal"}}
    };

    void MipsIvInterpreter::decodeRegimm(u32 opcode, InvokeOpInfo& codes, EeInstructionSet& set) {
        auto imm{static_cast<MipsRegImmOpcodes>((opcode >> 16) & 0x1f)};

        if (mapMipsRegimm.contains(imm)) {
            codes.execute = [imm](InvokeOpInfo& info) {
                mapMipsRegimm[imm].instHandler(info.ops);
            };
            set.r9OpcodeStr = mapMipsRegimm[imm].instName;
        }
    }

    EeCop MipsIvInterpreter::mapMipsCop{
        {Cop0Mfc, {c0mfc, "mfc"}},
        {Cop0Mtc, {c0mtc, "mtc"}},
        {Cop0Bc0, {copbc0tf, "bcXtf"}},

        {CopOp2Tlbr, {tlbr, "tlbr"}},
        {CopOp2Eret, {eret, "eret"}},

        {CopOp2Ei, {ei, "ei"}},
        {CopOp2Di, {di, "di"}}
    };

    void MipsIvInterpreter::decodeCop(u32 opcode, InvokeOpInfo& codes, EeInstructionSet& set) {
        codes.pipe = OutOfOrder::Cop0;
        const u8 cop{static_cast<u8>((opcode >> 26) & 0x3)};
        const u32 op{(opcode >> 21) & 0x1f};
        MipsIvCops copOp{};

        if (cop == 2 && op > 0x10) {
        } else {
            auto subOp{op | (cop * 0x100)};

            switch (subOp) {
            case Cop0Mfc:
            case Cop0Mtc:
            case Cop0Bc0:
                copOp = static_cast<MipsIvCops>(subOp); break;
            case CopOp2Opcodes:
                auto op2{static_cast<u8>(opcode & 0x3f)};
                copOp = static_cast<MipsIvCops>(op2);
                switch (op2) {
                case CopOp2Eret:
                    codes.pipe = OutOfOrder::Eret;
                    break;
                }
            }
        }
        if (mapMipsCop.contains(copOp)) {
            codes.execute = [copOp](InvokeOpInfo& info) {
                mapMipsCop[copOp].instHandler(info.ops);
            };
            set.r9OpcodeStr = mapMipsCop[copOp].instName;
        }
    }
    EeBase MipsIvInterpreter::mapMipsBase {
        {Bne, {bne, "bne"}},
        {Addi, {addi, "addi"}},
        {Slti, {slti, "slti"}},
        {Lui, {lui, "lui"}},

        {Lb, {lb, "lb"}},
        {Lh, {lh, "lh"}},
        {Lw, {lw, "lw"}},
        {Lbu, {lbu, "lbu"}},
        {Lhu, {lhu, "lhu"}},
        {Lwu, {lwu, "lwu"}},
        {Cache, {cache, "cache"}},
        {Nop, {nop, "nop"}},
        {Ld, {ld, "ld"}},
        {Sw, {sw, "sw"}}
    };
    void MipsIvInterpreter::decodeEmotion(u32 r9Inst, InvokeOpInfo& microCodes) {
        std::array<u8, 3> operands{
            EeOpcodeTranslator::getRegisters(r9Inst)};
        EeInstructionSet set{.code = r9Inst & 0x3f000000};
        const u32 offsetOrBase{r9Inst & 0x0000ffff};

        microCodes.ops = Operands{r9Inst, operands};
        switch (r9Inst >> 26) {
        case SpecialOpcodes:
            decodeSpecial(r9Inst, microCodes, set);
            break;
        case RegImmOpcodes:
            decodeRegimm(r9Inst, microCodes, set);
            break;
        case CopOpcodes:
            decodeCop(r9Inst, microCodes, set);
            break;
        case Ori:
            set.hasOffset = true;
        }
        std::array<std::string, 3> tagged{
            std::string("") + eeAllGprIdentifier[operands.at(0)],
            std::string("") + eeAllGprIdentifier[operands.at(1)],
            std::string("") + eeAllGprIdentifier[operands.at(2)],
        };
        const auto thirdOpArg{set.hasOffset ? tagged[2] : fmt::format("{:x}", offsetOrBase)};
        std::string r92Str{""};

        if (set.extraReg || set.hasOffset) {
            r92Str = fmt::format("{} {},{},{}", set.r9OpcodeStr, tagged[0], tagged[1], thirdOpArg);
        } else {
            r92Str = fmt::format("{} {},{}", set.r9OpcodeStr, tagged[0], tagged[1]);
        }
        auto baseOps{static_cast<MipsIvOpcodes>(r9Inst >> 26)};
        if (mapMipsBase.contains(baseOps)) {
            microCodes.execute = [baseOps](InvokeOpInfo& info) {
                mapMipsBase[baseOps].instHandler(info.ops);
            };

            user->debug("(MIPS) Opcode value {} at PC address {} decoded to {}", r9Inst, *cpu->eePc, r92Str);
        }
        if (!microCodes.execute) {
            microCodes.execute = [r92Str](InvokeOpInfo& err) {
                throw AppErr("Currently, we cannot handle the operation {} at PC address {}", r92Str, *cpu->eePc);
            };
        }
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

    Ref<engine::EeMipsCore> MipsIvInterpreter::cpu;
    Ref<vm::EmuVm> MipsIvInterpreter::vm;
    Ref<engine::FpuCop> MipsIvInterpreter::fpu;
    Ref<engine::copctrl::CtrlCop> MipsIvInterpreter::control;
}