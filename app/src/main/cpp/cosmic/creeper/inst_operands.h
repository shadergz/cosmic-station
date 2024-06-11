// SPDX-short-identifier: MIT, Version N/A
// This file is protected by the MIT license (please refer to LICENSE.md before making any changes, copying, or redistributing this software)
#pragma once

#include <common/types.h>
#include <array>
#include <unordered_map>
namespace cosmic::creeper {
    constexpr u8 first{0};
    constexpr u8 second{1};
    constexpr u8 third{2};

    using Reg = u8;

    class VuMicroOperands {
    public:
        VuMicroOperands() = default;
        VuMicroOperands(u32 vui) :
            inst(vui) {
            fd = (vui >> 6) & 0x1f;
            fs = (vui >> 11) & 0x1f;
            bc = (vui >> 16) & 0x1f;
            field = (vui >> 21) & 0x1f;
        }
        union {
            u32 inst;
        };
        Reg fd, fs;
        union {
            Reg bc, ft;
        };
        Reg field;
    };

    class Operands {
    public:
        Operands() = default;
        explicit Operands(u32 opcode, std::array<u8, 3>& ops) :
            gprs(ops) {
            inst = opcode;
        }
        union {
            u32 inst;
            i32 sins;
            std::array<u8, 4> pa8;
            std::array<i8, 4> ps8;

            std::array<u16, 2> pa16;
            std::array<i16, 2> ps16;
        };
        union {
            std::array<Reg, 3> gprs;
            struct {
                // RD = DEST (11), RT = TEST (16), RS = BASE (21)
                Reg rd, rt;
                union { Reg rs; Reg base; };
            };
        };
    };
    static_assert(sizeof(Operands) == 8, "");

    using OpcodeListAlternative = std::array<std::string, 3>;
    using OpcodeMapType = std::unordered_map<u64, OpcodeListAlternative>;

    struct EeInstructionSet {
        std::string instruction{"?"};
    };
    class EeOpcodeTranslator{
    public:
        static std::array<std::string, 3> interpreters;
        static std::array<std::string, 1> eeOps;
        static std::array<std::string, 1> iopOps;
        static std::array<std::string, 1> vuOps;
        static OpcodeMapType eeMipsCoreFmt;

        static auto getRegisters(u32 instruction) {
            std::array<Reg, 3> ops;
            ops[0] = (instruction >> (11 + 0 * 5)) & 0x1f;
            ops[1] = (instruction >> (11 + 1 * 5)) & 0x1f;
            ops[2] = (instruction >> (11 + 2 * 5)) & 0x1f;

            return ops;
        }
    };

    class GenericDisassembler {
    public:
        enum ConvMode {
            Mips,
            Psx,
            Vu
        };

        GenericDisassembler(std::span<const std::string> alias, u32 pc, ConvMode ctrl);
        void convMicro2Str(Operands& opsList, std::string& output, std::string_view& format);
    private:
        std::span<const std::string> registersAlias;
        void mips2Str(std::string& output, std::string_view& format);

        u32 currPc;
        ConvMode dis;
        Operands ops;
    };
}
