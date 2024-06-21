// SPDX-short-identifier: MIT, Version N/A
// This file is protected by the MIT license (please refer to LICENSE.md before making any changes, copying, or redistributing this software)
#include <common/global.h>
#include <creeper/cached_blocks.h>
#include <ee/ee_core.h>

#include <range/v3/all.hpp>
namespace cosmic::creeper {
    using namespace ee;

    EeMapSpecial MipsIvInterpreter::ivSpecial{
        {SpecialSll, {&sll, "SLL"}},
        {SpecialSrl, {&srl, "SRL"}},
        {SpecialSra, {&sra, "SRA"}},
        {SpecialSllv, {&sllv, "SLLV"}},
        {SpecialSrlv, {&srlv, "SRLV"}},
        {SpecialSrav, {&srav, "SRAV"}},
        {SpecialJr, {&jr, "JR"}},
        {SpecialJalr, {&jalr, "JALR $RD, $RS"}},
        {SpecialMovZ, {&movz, "MOVZ"}},
        {SpecialMovN, {&movn, "MOVN"}},
        {SpecialSyscall, {&iSyscall, "SYSCALL"}},
        {SpecialBreak, {&iBreak, "BREAK"}},
        {SpecialSync, {&nop, "SYNC"}},

        {SpecialMult, {&mult, "MULT"}},
        {SpecialMultu, {&multu, "MULTU"}},

        {SpecialDiv, {&div, "DIV"}},
        {SpecialDivu, {&divu, "DIVU"}},

        {SpecialAdd, {&add, "ADD"}},
        {SpecialAddu, {&addu, "ADDU"}},
        {SpecialSub, {&sub, "SUB"}},
        {SpecialSubu, {&subu, "SUBU"}},
        {SpecialAnd, {&iAnd, "AND"}},
        {SpecialOr, {&iOr, "OR"}},
        {SpecialXor, {&iXor, "XOR"}},
        {SpecialNor, {&nor, "NOR"}},

        {SpecialDAdd, {&dadd, "DADD"}},
        {SpecialDAddu, {&daddu, "DADDU"}},
        {SpecialDSub, {&dsub, "DSUB"}},
        {SpecialDSubu, {&dsubu, "DSUBU"}},
        {SpecialSlt, {&slt, "SLT"}}
    };

    void MipsIvInterpreter::decodeSpecial(u32 opcode,
        InvokeOpInfo& codes, EeInstructionSet& set) {
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
        {RegImmBltzal, {&bltzal, "BLTZAL"}}
    };

    void MipsIvInterpreter::decodeRegimm(
            u32 opcode, InvokeOpInfo& codes, EeInstructionSet& set) {
        auto regImm{static_cast<MipsRegImmOpcodes>((opcode >> 16) & 0x1f)};
        getOpcodeHandler(ivRegImm, regImm, codes, set);
    }

    EeCop MipsIvInterpreter::ivCop{
        {Cop0Mfc, {&c0mfc, "MFC"}},
        {Cop0Mtc, {&c0mtc, "MTC"}},
        {Cop0Bc0, {&copbc0tf, "BC0TF"}},
    };
    EeCopOp2 MipsIvInterpreter::ivCopOp2{
        {CopOp2Tlbr, {&tlbr, "TLBR"}},
        {CopOp2Tlbwi, {&tlbwi, "TLBWI"}},
        {CopOp2Eret, {&eret, "ERET"}},

        {CopOp2Ei, {&ei, "EI"}},
        {CopOp2Di, {&di, "DI"}}
    };

    void MipsIvInterpreter::decodeCop(u32 opcode, InvokeOpInfo& codes, EeInstructionSet& set) {
        codes.pipe = OutOfOrder::Cop0;
        const auto cop{(opcode >> 26) & 0x3};
        const auto op{(opcode >> 21) & 0x1f};
        MipsIvCops copOp{};

        if (cop == 2 && op >= 0x10) {
        } else {
            auto subOp{op | (cop * 0x100)};

            switch (subOp) {
            case Cop0Mfc:
            case Cop0Mtc:
            case Cop0Bc0:
                copOp = static_cast<MipsIvCops>(subOp); break;
            case CopOp2Opcodes:
                auto op2{opcode & 0x3f};
                switch (op2) {
                case CopOp2Eret:
                    codes.pipe = OutOfOrder::Eret; break;
                }
                getOpcodeHandler(ivCopOp2, static_cast<MipsIvCopOp2>(op2), codes, set);
                return;
            }
        }
        getOpcodeHandler(ivCop, copOp, codes, set);
    }
    EeCore MipsIvInterpreter::ivCore {
        {Bne, {&bne, "BNE $RS, $RT, $IMM"}},
        {Addi, {&addi, "ADDI"}},
        {Addiu, {&addiu, "ADDIU"}},
        {Slti, {&slti, "SLTI"}},
        {Ori, {&ori, "ORI"}},
        {Xori, {&xori, "XORI"}},
        {Lui, {&lui, "LUI"}},

        {Lb, {&lb, "LB"}},
        {Lh, {&lh, "LH"}},
        {Lw, {&lw, "LW"}},
        {Lbu, {&lbu, "LBU"}},
        {Lhu, {&lhu, "LHU"}},
        {Lwu, {&lwu, "LWU"}},
        {Sw, {&sw, "SW"}},
        {Cache, {&cache, "CACHE"}},
        {Nop, {&nop, "NOP"}},
        {Ld, {&ld, "LD"}}
    };
    void MipsIvInterpreter::decodeEmotion(u32 opcode, InvokeOpInfo& microCodes) {
        auto operands{
            EeOpcodeTranslator::getRegisters(opcode)};
        EeInstructionSet set{};

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
        auto coreOps{static_cast<MipsIvOpcodes>(opcode >> 26)};
        getOpcodeHandler(ivCore, coreOps, microCodes, set);

        if (!microCodes.execute) {
            microCodes.execute = [](Operands& err) {
                throw AppErr("Currently, we cannot handle the operation {:#x} at PC address {:#x}", err.inst, static_cast<u32>(cpu->eePc));
            };
            return;
        }
        user->debug("(MIPS) Opcode value {:#x} at PC address {:#x} decoded to {}", opcode, actualPc, set.instruction);
    }
    void MipsIvInterpreter::getOpcodeHandler(auto opcodes, auto micro,
        InvokeOpInfo& info, EeInstructionSet& set) {
        if (!opcodes.contains(micro))
            return;
        auto opc{opcodes.find(micro)};
        if (opc == opcodes.end())
            return;
        info.execute = (opc->second).opcodeHandler;
        std::string_view format{opc->second.opcodeFormat};

        auto backSlash{ranges::find(format, ' ')};
        u64 firstBack{};
        if (backSlash != format.end()) {
            firstBack = static_cast<u64>(std::distance(std::begin(format), backSlash));
        }

        if (!firstBack) {
            set.instruction = format;
            return;
        } else {
            set.instruction = format.substr(0, firstBack);
        }
        GenericDisassembler disassembler{
            eeAllGprIdentifier,
            actualPc,
            GenericDisassembler::Mips
        };

        format = format.substr(firstBack);
        disassembler.convMicro2Str(info.ops, set.instruction, format);
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
    Optional<EeMipsCore> MipsIvInterpreter::cpu;
    Optional<vm::EmuVm> MipsIvInterpreter::vm;
    Optional<FpuCop> MipsIvInterpreter::fpu;
    Optional<CtrlCop> MipsIvInterpreter::c0;

    u32& MipsIvInterpreter::doReg(const Reg regId) {
        return cpu->GPRs[regId].words[0];
    }
    i32& MipsIvInterpreter::signedDoReg(const Reg regId) {
        return cpu->GPRs[regId].swords[0];
    }
    u64& MipsIvInterpreter::do64Reg(const Reg regId) {
        return cpu->GPRs[regId].dw[0];
    }
    i64& MipsIvInterpreter::signedDo64Reg(const Reg regId) {
        return cpu->GPRs[regId].sdw[0];
    }
}