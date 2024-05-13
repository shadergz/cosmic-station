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

    class VuMicroOperands {
    public:
        VuMicroOperands() = default;
        VuMicroOperands(u32 vui) :
            inst(vui) {
            fd = (vui >> 6) & 0x1f;
            fs = (vui >> 11) & 0x1f;
            ft = (vui >> 16) & 0x1f;
            dest = (vui >> 21) & 0x1f;
        }
        union {
            u32 inst;
        };
        u8 fd, fs, ft, dest;
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
            std::array<u16, 2> pa16;
            std::array<i16, 2> ps16;
        };
        union {
            std::array<u8, 3> gprs;
            struct {
                // RS = SOURCE (11), RD = BASE (16), RT = TEST (21)
                u8 rd, rs, rt;
            };
        };
    };
    using OpcodeListAlternative = std::array<const char*, 3>;
    using OpcodeMapType = std::unordered_map<u64, OpcodeListAlternative>;

    struct EeInstructionSet {
        std::string opcodeStr{"@@@"};
        bool extraParameter;
    };
    class EeOpcodeTranslator{
    public:
        static std::array<const char*, 3> interpreters;
        static std::array<const char*, 1> eeOps;
        static std::array<const char*, 1> iopOps;
        static std::array<const char*, 1> vuOps;
        static OpcodeMapType eeMipsCoreFmt;

        static auto getRegisters(u32 r9Inst) {
            std::array<u8, 3> ops;
            ops[0] = (r9Inst >> (11 + 0 * 5)) & 0x1f;
            ops[1] = (r9Inst >> (11 + 1 * 5)) & 0x1f;
            ops[2] = (r9Inst >> (11 + 2 * 5)) & 0x1f;

            return ops;
        }
    };
}
