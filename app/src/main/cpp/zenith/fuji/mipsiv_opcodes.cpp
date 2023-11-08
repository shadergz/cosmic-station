#include <fuji/mipsiv_interpreter.h>
#include <eeiv/ee_engine.h>

#define QualifiedSpecial(thiz, r, value, op)\
    [thiz, r, value]() {\
        op(value, r[0], r[1], r[2]);\
    }
#define Qualified3(thiz, r, value, op)\
    [thiz, r, value]() {\
        op(value, r[0], r[1]);\
    }

namespace zenith::fuji {
    std::function<void()> MipsIVInterpreter::decodeSpecialISA(u32 opcode) {
        std::array<u32*, 3> regs{
            mainMips.GprAt<u32 *>(opcode >> 11 & 0x1f),
            mainMips.GprAt<u32*>(opcode >> 16 & 0x1f),
            mainMips.GprAt<u32*>(opcode >> 21 & 0x1f)};

        switch (opcode & 0x3f) {
        case MipsIVOpcodes::SpecialSlt:
            return QualifiedSpecial(this, regs, opcode, slt);
        case MipsIVOpcodes::SpecialXor:
            return QualifiedSpecial(this, regs, opcode, ivXor);
        }
        return {};
    }

    std::function<void()> MipsIVInterpreter::decodeFunc(u32 opcode) {
        u32 gprs[2]{opcode >> 16, opcode >> 21};
        gprs[0] = gprs[0] & 0x1f;
        gprs[1] = gprs[1] & 0x1f;
        std::array<u32*, 2> gprRef{mainMips.GprAt<u32*>(gprs[0]), mainMips.GprAt<u32*>(gprs[1])};

        switch (opcode >> 26) {
        case MipsIVOpcodes::SpecialOpcodes:
            return decodeSpecialISA(opcode);
        case MipsIVOpcodes::Addi:
            return Qualified3(this, gprRef, opcode, addi);
        case MipsIVOpcodes::Slti:
            return Qualified3(this, gprRef, opcode, slti);
        }
        return {};
    }
    u32 MipsIVInterpreter::fetchFromPc() {
        if (*mainMips.eePC & 4095)
            ;
        u32 save{mainMips.cyclesToWaste};
        u32 opcode{mainMips.fetchByPC()};

        mainMips.cyclesToWaste = save;
        return opcode;
    }
}