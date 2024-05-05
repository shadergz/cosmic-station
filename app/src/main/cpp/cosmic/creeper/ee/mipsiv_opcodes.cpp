// SPDX-short-identifier: MIT, Version N/A
// This file is protected by the MIT license (please refer to LICENSE.md before making any changes, copying, or redistributing this software)
#include <common/global.h>
#include <creeper/ee/mipsiv_cached.h>
#include <engine/ee_core.h>
#define TRANSLATE_REGISTERS 0
namespace cosmic::creeper::ee {
    using namespace engine;

    EeMapSpecial MipsIvInterpreter::mapMipsSpecial{
        {SpecialSll, sll},
        {SpecialSrl, srl},
        {SpecialSra, sra},
        {SpecialSllv, sllv},
        {SpecialSrlv, srlv},
        {SpecialSrav, srav},
        {SpecialMovZ, movz},
        {SpecialMovN, movn},
        {SpecialSyscall, ivSyscall},
        {SpecialBreak, ivBreak},

        {SpecialMult, mult},
        {SpecialMultu, multu},

        {SpecialDiv, div},
        {SpecialDivu, divu},

        {SpecialAdd, add},
        {SpecialAddu, addu},
        {SpecialSub, sub},
        {SpecialSubu, subu},
        {SpecialDAdd, dadd},
        {SpecialDAddu, daddu},
        {SpecialDSub, dsub},
        {SpecialDSubu, dsubu},
        {SpecialXor, ivXor},
        {SpecialSlt, slt}
    };

    InvokableCached MipsIvInterpreter::execSpecial(u32 opcode, InvokeOpInfo& decode) {
        switch (opcode & 0x3f) {
        case SpecialSll ... SpecialBreak:
            return [opcode](InvokeOpInfo& info) {
                mapMipsSpecial[static_cast<MipsIvSpecial>(opcode & 0x3f)](info.ops);
            };
        case SpecialMult:
        case SpecialMultu: {
            decode.pipe = OutOfOrder::EffectivePipeline::Mac0;
            decode.extraCycles = Mul;
            return [opcode](InvokeOpInfo& info) {
                mapMipsSpecial[static_cast<MipsIvSpecial>(opcode & 0x3f)](info.ops);
            };
        }
        case SpecialDiv:
        case SpecialDivu: {
            decode.pipe = OutOfOrder::EffectivePipeline::Mac0;
            decode.extraCycles = Div;

            return [opcode](InvokeOpInfo& info) {
                mapMipsSpecial[static_cast<MipsIvSpecial>(opcode & 0x3f)](info.ops);
            };
        }
        case SpecialAdd ... SpecialSlt:
            return [opcode](InvokeOpInfo& info) {
                mapMipsSpecial[static_cast<MipsIvSpecial>(opcode & 0x3f)](info.ops);
            };
        }
        return {};
    }
    EeRegImm MipsIvInterpreter::mapMipsRegimm{
        {RegImmBltzal, bltzal}
    };
    InvokableCached MipsIvInterpreter::execRegimm(u32 opcode, InvokeOpInfo& decode) {
        const u32 opImm{opcode >> 16 & 0x1f};
        switch (opImm) {
        case RegImmBltzal:
            return [opImm](InvokeOpInfo& info) {
                mapMipsRegimm[static_cast<MipsRegImmOpcodes>(opImm)](info.ops);
            };
        }
        return {};
    }

    EeCop MipsIvInterpreter::mapMipsCop{
        {Cop0Mfc, c0mfc},
        {Cop0Mtc, c0mtc},
        {Cop0Bc0, copbc0tf},

        {CopOp2Tlbr, tlbr},
        {CopOp2Eret, eret},

        {CopOp2Ei, ei},
        {CopOp2Di, di}
    };

    InvokableCached MipsIvInterpreter::execCop(u32 opcode, InvokeOpInfo& decode) {
        decode.pipe = OutOfOrder::Cop0;
        u8 cop{static_cast<u8>((opcode >> 26) & 0x3)};
        u32 op{(opcode >> 21) & 0x1f};
        if (cop == 2 && op > 0x10) {
        } else {
            switch (op | (cop * 0x100)) {
            case Cop0Mfc:
            case Cop0Mtc:
            case Cop0Bc0:
                return [op, cop](InvokeOpInfo& info) {
                    mapMipsCop[static_cast<MipsIvCops>(op | (cop * 0x100))](info.ops);
                };
            case CopOp2Opcodes:
                u8 op2{static_cast<u8>(opcode & 0x3f)};
                switch (op2) {
                case CopOp2Tlbr:
                    return [op2](InvokeOpInfo& info) {
                        mapMipsCop[static_cast<MipsIvCops>(op2)](info.ops);
                    };
                case CopOp2Eret: {
                    decode.pipe = OutOfOrder::Eret;
                    return [op2](InvokeOpInfo& info) {
                        mapMipsCop[static_cast<MipsIvCops>(op2)](info.ops);
                    };
                }
                case CopOp2Ei:
                case CopOp2Di:
                    return [op2](InvokeOpInfo& info) {
                        mapMipsCop[static_cast<MipsIvCops>(op2)](info.ops);
                    };
                }
            }
        }
        return {};
    }
    EeBase MipsIvInterpreter::mapMipsBase {
        {Bne, bne},
        {Addi, addi},
        {Slti, slti},
        {Lui, lui},

        {Lb, lb},
        {Lh, lh},
        {Lw, lw},
        {Lbu, lbu},
        {Lhu, lhu},
        {Lwu, lwu},
        {Cache, cache},
        {Nop, nop},
        {Ld, ld},
        {Sw, sw}
    };
    thread_local std::array<const char*, 3> translatedGprs{"Unk", "Unk", "Unk"};

    void MipsIvInterpreter::execBlackBox(u32 opcode, InvokeOpInfo& microCodes) {
        std::array<u8, 3> operands;
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
        microCodes.ops = Operands{opcode, operands};
        microCodes.execute = [](InvokeOpInfo& err) {
            std::array<std::string, 3> gprs{
                std::string{"%"} + gprsNames[err.ops.rd],
                std::string{"%"} + gprsNames[err.ops.rt],
                std::string{"%"} + gprsNames[err.ops.rs]
            };
            throw AppErr("Invalid or unrecognized opcode {:#x}, parameters: INV {}",
                err.ops.inst, fmt::join(gprs, ", "));
        };
        switch (opcode >> 26) {
        case SpecialOpcodes:
            microCodes.execute = execSpecial(opcode, microCodes); break;
        case RegImmOpcodes:
            microCodes.execute = execRegimm(opcode, microCodes); break;
        case Bne ... Slti:
        /*
        case Sltiu:
        case Andi:
        case Ori:
        case Xori:
        */
        case Lui:
            microCodes.execute = [opcode](InvokeOpInfo& info) {
                mapMipsBase[static_cast<MipsIvOpcodes>(opcode >> 26)](info.ops);
            };
            break;
        case CopOpcodes:
            microCodes.execute = execCop(opcode, microCodes); break;
        case Lb ... Sw:
            microCodes.execute = [opcode](InvokeOpInfo& info) {
                mapMipsBase[static_cast<MipsIvOpcodes>(opcode >> 26)](info.ops);
            };
            break;
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