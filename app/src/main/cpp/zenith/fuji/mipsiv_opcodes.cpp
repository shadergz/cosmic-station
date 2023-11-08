#include <fuji/mipsiv_interpreter.h>
#include <eeiv/ee_engine.h>

#define QualifiedSpecial(thiz, r, value, op)\
    [thiz, r, value]() {\
        op(value, r[0], r[1], r[2]);\
    }
#define Qualified3(thiz, r, value, op)\
    [thiz, r, value]() {\
        op(value, r[1], r[2]);\
    }

namespace zenith::fuji {
    std::function<void()> MipsIVInterpreter::decodeFunc(u32 opcode) {
        u32 gprs[3]{opcode >> 11, opcode >> 16, opcode >> 21};
        gprs[0] = gprs[0] & 0x1f;
        gprs[1] = gprs[1] & 0x1f;
        gprs[2] = gprs[2] & 0x1f;
        std::array<u32*, 3> gprRef{
            mainMips.GprAt<u32*>(gprs[0]),
            mainMips.GprAt<u32*>(gprs[1]),
            mainMips.GprAt<u32*>(gprs[2])};

        std::function<void()> op{};
#define SWQualified(saveHere, level, op)\
    saveHere = level(this, gprRef, opcode, op);\
    break

        switch (opcode >> 26) {
        case MipsIVOpcodes::SpecialOpcodes: {
            switch (opcode & 0x3f) {
            case MipsIVOpcodes::SpecialSlt:
                SWQualified(op, QualifiedSpecial, slt);
            case MipsIVOpcodes::SpecialXor:
                SWQualified(op, QualifiedSpecial, ivXor);
            }
        }
        case MipsIVOpcodes::Addi:
            SWQualified(op, Qualified3, addi);
        case MipsIVOpcodes::Slti:
            SWQualified(op, Qualified3, slti);
        case MipsIVOpcodes::Bltzal:
            SWQualified(op, Qualified3, bltzal);
        case MipsIVOpcodes::Sw:
            SWQualified(op, Qualified3, sw);
        }
        return op;
#undef SWQualified
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