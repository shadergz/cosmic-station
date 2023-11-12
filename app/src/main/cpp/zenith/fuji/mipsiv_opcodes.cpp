#include <fuji/mipsiv_interpreter.h>
#include <eeiv/ee_engine.h>

#define QualifiedSpecial(thiz, op)\
    [thiz](InvokeOpInfo& info) {\
        op(info.value, info.regs[0], info.regs[1], info.regs[2]);\
    }
#define Qualified3(thiz, op)\
    [thiz](InvokeOpInfo& info) {\
        op(info.value, info.regs[1], info.regs[2]);\
    }

namespace zenith::fuji {
    InvokeOpInfo MipsIVInterpreter::decodeFunc(u32 opcode) {
        InvokeOpInfo decoded{.value = static_cast<i32>(opcode)};
        decoded.ids[0] = opcode >> 11 & 0x1f;
        decoded.regs[0] = mainMips.GprAt<u32*>(decoded.ids[0]);

        decoded.ids[1] = opcode >> 16 & 0x1f;
        decoded.regs[1] = mainMips.GprAt<u32*>(decoded.ids[1]);

        decoded.ids[2] = opcode >> 21 & 0x1f;
        decoded.regs[2] = mainMips.GprAt<u32*>(decoded.ids[2]);

#define SWQualified(level, op)\
    decoded.execute = level(this, op);\
    break

        switch (opcode >> 26) {
        case SpecialOpcodes:
            switch (opcode & 0x3f) {
            case SpecialSlt: SWQualified(QualifiedSpecial, slt);
            case SpecialXor: SWQualified(QualifiedSpecial, ivXor);
            }
            break;

        case RegImmOpcodes:
            switch (opcode >> 16 & 0x1f) {
            case RegImmBltzal:
                SWQualified(Qualified3, bltzal);
            }
            break;

        case Addi:  SWQualified(Qualified3, addi);
        case Slti:  SWQualified(Qualified3, slti);

        case CopOpcodes: {
            decoded.pipe = OutOfOrder::Cop0;
            u8 cop{static_cast<u8>((opcode >> 26) & 0x3)};
            u32 op{(opcode >> 21) & 0x1f};
            if (cop == 2 && op > 0x10) {

            } else {
                switch (op | (cop * 0x100)) {
                case CopOp2:
                    u8 op2{static_cast<u8>(opcode & 0x3f)};
                    switch (op2) {
                    case CopOp2Tlbr: SWQualified(Qualified3, tlbr);
                    case CopOp2Eret:
                        decoded.pipe = OutOfOrder::Eret;
                        SWQualified(Qualified3, eret);
                    case CopOp2Ei:   SWQualified(Qualified3, ei);
                    case CopOp2Di:   SWQualified(Qualified3, di);
                    }
                }
            }
            break;
        }
        case Lb:    SWQualified(Qualified3, lb);
        case Lh:    SWQualified(Qualified3, lh);
        case Lw:    SWQualified(Qualified3, lw);
        case Lbu:   SWQualified(Qualified3, lbu);
        case Lhu:   SWQualified(Qualified3, lhu);
        case Lwu:   SWQualified(Qualified3, lwu);
        case Cache: SWQualified(Qualified3, cache);
        case Nop:   SWQualified(Qualified3, nop);
        case Ld:    SWQualified(Qualified3, ld);
        case Sw:    SWQualified(Qualified3, sw);
        }

        return decoded;
#undef SWQualified
    }
    u32 MipsIVInterpreter::fetchPcInst() {
        if (*mainMips.eePC & 4095)
            ;
        u32 save{mainMips.cyclesToWaste};
        u32 opcode{mainMips.fetchByPC()};

        mainMips.cyclesToWaste = save;
        return opcode;
    }
}