#include <fuji/mipsiv_interpreter.h>
#include <eeiv/ee_engine.h>

#define SWCached(op)\
    decode.execute = [this](InvokeOpInfo& info) {\
        op(info.ops);\
    };\
    break

namespace zenith::fuji {
    u32 MipsIVInterpreter::decMipsIvS(u32 opcode, InvokeOpInfo& decode) {
        switch (opcode & 0x3f) {
        case SpecialBreak: SWCached(iBreak);
        case SpecialXor:   SWCached(ivXor);
        case SpecialSlt:   SWCached(slt);
        }
        return opcode & 0x3f;
    }
    u32 MipsIVInterpreter::decMipsIvRegImm(u32 opcode, InvokeOpInfo& decode) {
        u32 opImm{opcode >> 16 & 0x1f};
        switch (opImm) {
        case RegImmBltzal:
            SWCached(bltzal);
        }
        return opImm;
    }
    u32 MipsIVInterpreter::decMipsIvCop0(u32 opcode, InvokeOpInfo& decode) {
        decode.pipe = OutOfOrder::Cop0;
        u8 cop{static_cast<u8>((opcode >> 26) & 0x3)};
        u32 op{(opcode >> 21) & 0x1f};
        if (cop == 2 && op > 0x10) {

        } else {
            switch (op | (cop * 0x100)) {
            case Cop0Mfc: SWCached(c0mfc);
            case Cop0Mtc: SWCached(c0mtc);
            case Cop0Bc0: SWCached(copbc0tf);
            case CopOp2:
                u8 op2{static_cast<u8>(opcode & 0x3f)};
                switch (op2) {
                case CopOp2Tlbr: SWCached(tlbr);
                case CopOp2Eret:
                    decode.pipe = OutOfOrder::Eret;
                    SWCached(eret);
                case CopOp2Ei:   SWCached(ei);
                case CopOp2Di:   SWCached(di);
                }
            }
        }
        return op;
    }

    InvokeOpInfo MipsIVInterpreter::decMipsBlackBox(u32 opcode) {
        InvokeOpInfo decode{};
        std::array <u8, 3> operands{};
        operands[0] = opcode >> 11 & 0x1f;
        operands[1] = opcode >> 16 & 0x1f;
        operands[2] = opcode >> 21 & 0x1f;

        decode.ops = Operands(opcode, operands);

        switch (opcode >> 26) {
        case SpecialOpcodes:
            decMipsIvS(opcode, decode);
            break;
        case RegImmOpcodes:
            decMipsIvRegImm(opcode, decode);
            break;
        case Addi:  SWCached(addi);
        case Slti:  SWCached(slti);
        case CopOpcodes:
            decMipsIvCop0(opcode, decode);
            break;
        case Lb:    SWCached(lb);
        case Lh:    SWCached(lh);
        case Lw:    SWCached(lw);
        case Lbu:   SWCached(lbu);
        case Lhu:   SWCached(lhu);
        case Lwu:   SWCached(lwu);
        case Cache: SWCached(cache);
        case Nop:   SWCached(nop);
        case Ld:    SWCached(ld);
        case Sw:    SWCached(sw);
        }

        return decode;
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