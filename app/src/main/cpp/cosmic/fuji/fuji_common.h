// SPDX-short-identifier: MIT, Version N/A
// This file is protected by the MIT license (please refer to LICENSE.md before making any changes, copying, or redistributing this software)
#pragma once

#include <common/types.h>
#define IV_FUJI_OP(op)\
    void op(Operands ops)

namespace cosmic::fuji {
    constexpr u8 first{0};
    constexpr u8 second{1};
    constexpr u8 third{2};

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
}
