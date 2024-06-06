// SPDX-short-identifier: MIT, Version N/A
// This file is protected by the MIT license (please refer to LICENSE.md before making any changes, copying, or redistributing this software)
#include <common/global.h>
#include <creeper/cached_blocks.h>
#include <engine/ee_core.h>

namespace cosmic::creeper {
    using namespace engine;

    EeMapSpecial MipsIvInterpreter::ivSpecial{
        {SpecialSll, {&sll, "sll"}},
        {SpecialSrl, {&srl, "srl"}},
        {SpecialSra, {&sra, "sra"}},
        {SpecialSllv, {&sllv, "sllr"}},
        {SpecialSrlv, {&srlv, "srlv"}},
        {SpecialSrav, {&srav, "srav"}},
        {SpecialJr, {&jr, "jr"}},
        {SpecialMovZ, {&movz, "movz"}},
        {SpecialMovN, {&movn, "movn"}},
        {SpecialSyscall, {&iSyscall, "syscall"}},
        {SpecialBreak, {&iBreak, "break"}},
        {SpecialSync, {&nop, "sync"}},

        {SpecialMult, {&mult, "mult"}},
        {SpecialMultu, {&multu, "multu"}},

        {SpecialDiv, {&div, "div"}},
        {SpecialDivu, {&divu, "divu"}},

        {SpecialAdd, {&add, "add"}},
        {SpecialAddu, {&addu, "addu"}},
        {SpecialSub, {&sub, "sub"}},
        {SpecialSubu, {&subu, "subu"}},
        {SpecialAnd, {&iAnd, "and"}},
        {SpecialOr, {&iOr, "or"}},
        {SpecialXor, {&iXor, "xor"}},
        {SpecialNor, {&nor, "nor"}},

        {SpecialDAdd, {&dadd, "dadd"}},
        {SpecialDAddu, {&daddu, "daadu"}},
        {SpecialDSub, {&dsub, "dsub"}},
        {SpecialDSubu, {&dsubu, "dsubu"}},
        {SpecialSlt, {&slt, "slt"}}
    };

    void MipsIvInterpreter::decodeSpecial(u32 opcode, InvokeOpInfo& codes, EeInstructionSet& set) {
        switch (opcode & 0x3f) {
        case SpecialJr:
            codes.pipe = OutOfOrder::EffectivePipeline::Branch;
            break;
        case SpecialSync:
            codes.pipe = OutOfOrder::EffectivePipeline::Sync;
            break;
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
        auto exclusive{static_cast<MipsIvSpecial>(opcode & 0x3f)};
        getOpcodeHandler(ivSpecial, exclusive, codes, set);
    }
    EeRegImm MipsIvInterpreter::ivRegImm{
        {RegImmBltzal, {&bltzal, "bltzal"}}
    };

    void MipsIvInterpreter::decodeRegimm(u32 opcode, InvokeOpInfo& codes, EeInstructionSet& set) {
        auto regImm{static_cast<MipsRegImmOpcodes>((opcode >> 16) & 0x1f)};
        getOpcodeHandler(ivRegImm, regImm, codes, set);
    }

    EeCop MipsIvInterpreter::ivCop{
        {Cop0Mfc, {&c0mfc, "mfc"}},
        {Cop0Mtc, {&c0mtc, "mtc"}},
        {Cop0Bc0, {&copbc0tf, "bcXtf"}},

        {CopOp2Tlbr, {&tlbr, "tlbr"}},
        {CopOp2Eret, {&eret, "eret"}},

        {CopOp2Ei, {&ei, "ei"}},
        {CopOp2Di, {&di, "di"}}
    };

    void MipsIvInterpreter::decodeCop(u32 opcode, InvokeOpInfo& codes, EeInstructionSet& set) {
        codes.pipe = OutOfOrder::Cop0;
        const auto cop{static_cast<u8>((opcode >> 26) & 0x3)};
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
                switch (op2) {
                case CopOp2Eret:
                    codes.pipe = OutOfOrder::Eret;
                    copOp = static_cast<MipsIvCops>(op2);
                    break;
                }
            }
        }
        getOpcodeHandler(ivCop, copOp, codes, set);
    }
    EeCore MipsIvInterpreter::ivCore {
        {Bne, {&bne, "bne"}},
        {Addi, {&addi, "addi"}},
        {Slti, {&slti, "slti"}},
        {Ori, {&ori, "ori"}},
        {Xori, {&xori, "xori"}},
        {Lui, {&lui, "lui"}},

        {Lb, {&lb, "lb"}},
        {Lh, {&lh, "lh"}},
        {Lw, {&lw, "lw"}},
        {Lbu, {&lbu, "lbu"}},
        {Lhu, {&lhu, "lhu"}},
        {Lwu, {&lwu, "lwu"}},
        {Sw, {&sw, "sw"}},
        {Cache, {&cache, "cache"}},
        {Nop, {&nop, "nop"}},
        {Ld, {&ld, "ld"}}
    };
    void MipsIvInterpreter::decodeEmotion(u32 opcode, InvokeOpInfo& microCodes) {
        std::array<u8, 3> operands{
            EeOpcodeTranslator::getRegisters(opcode)};
        EeInstructionSet set{};
        // const u32 offsetOrBase{opcode & 0x0000ffff};

        microCodes.ops = Operands{opcode, operands};
        switch (opcode >> 26) {
        case SpecialOpcodes:
            decodeSpecial(opcode, microCodes, set);
            break;
        case RegImmOpcodes:
            decodeRegimm(opcode, microCodes, set);
            break;
        case CopOpcodes:
            decodeCop(opcode, microCodes, set);
            break;
        }

        const auto& firstOp{eeAllGprIdentifier[operands.at(0)]};
        const auto& secondOp{eeAllGprIdentifier[operands.at(1)]};
        // const auto& third{eeAllGprIdentifier[operands.at(2)]};

        auto coreOps{static_cast<MipsIvOpcodes>(opcode >> 26)};
        getOpcodeHandler(ivCore, coreOps, microCodes, set);

        std::string decoded;
        decoded = fmt::format("{}\t{}, {}", set.instruction, firstOp, secondOp);

        if (!microCodes.execute) {
            microCodes.execute = [decoded](Operands& err) {
                throw AppErr("Currently, we cannot handle the operation {} at PC address {:x}", decoded, *cpu->eePc);
            };
            return;

        }
        user->debug("(MIPS) Opcode value {} at PC address {} decoded to {}", opcode, *cpu->eePc, decoded);
    }
    void MipsIvInterpreter::getOpcodeHandler(auto opcodes, auto micro,
        InvokeOpInfo& info, EeInstructionSet& set) {
        if (!opcodes.contains(micro))
            return;
        auto opc{opcodes.find(micro)};
        if (opc == opcodes.end())
            return;

        auto& handler{(opc->second).opcodeHandler};
        info.execute = handler;
        set.instruction = opc->second.nameHandler;
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
    Ref<engine::CtrlCop> MipsIvInterpreter::c0;

    u32& MipsIvInterpreter::doReg(auto regOp) {
        return cpu->GPRs[regOp].words[0];
    }
    i32& MipsIvInterpreter::signedDoReg(auto regOp) {
        return cpu->GPRs[regOp].swords[0];
    }
    u64& MipsIvInterpreter::do64Reg(auto regOp) {
        return cpu->GPRs[regOp].dw[0];
    }
    i64& MipsIvInterpreter::signedDo64Reg(auto regOp) {
        return cpu->GPRs[regOp].sdw[0];
    }
}