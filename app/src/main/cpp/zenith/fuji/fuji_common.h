#pragma once

#define IvFujiOp(op)\
    void op(Operands ops)

namespace zenith::fuji {
    constexpr u8 first{0};
    constexpr u8 second{1};
    constexpr u8 third{2};

    class Operands {
    public:
        Operands() = default;
        explicit Operands(u32 opcode, std::array<u8, 3>& ops)
            : gprs(ops) {
            operation.sins = static_cast<i32>(opcode);
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
            u8 fir, sec, thi;
        };
        instruction operation;
    };
}
