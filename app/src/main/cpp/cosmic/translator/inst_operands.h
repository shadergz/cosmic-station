// SPDX-short-identifier: MIT, Version N/A
// This file is protected by the MIT license (please refer to LICENSE.md before making any changes, copying, or redistributing this software)
#pragma once

#include <common/types.h>
#include <array>
namespace cosmic::translator {
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
        explicit Operands(u32 opcode, std::array<u8, 3>& ops)
            : gprs(ops) {
            operation.inst = opcode;
        }
        union instruction{
            u32 inst;
            i32 sins;
            std::array<u8, 4> pa8;
            std::array<u16, 2> pa16;
            std::array<i16, 2> ps16;
        };
        union {
            std::array<u8, 3> gprs;
            struct {
                u8 fir, sec, thi;
            };
        };
        instruction operation;
    };
    extern std::array<const char*, 3> opsNames;
    extern std::array<const char*, 2> interpreters;
    extern std::array<const char*, 1> eeOps;
    extern std::array<const char*, 1> psxOps;
}
