#include <fuji/mipsiv_interpreter.h>
#include <eeiv/ee_engine.h>

namespace zenith::fuji {
    void MipsIVInterpreter::addi(u32 fetched, u32* gprDest, u32* gprSrc) {
        *gprDest = fetched & 0xffff + *gprSrc;
    }

    std::function<void()> MipsIVInterpreter::decodeFunc(u32 opcode) {
        u32 gprs[2]{opcode >> 16, opcode >> 21};
        gprs[0] = gprs[0] & 0x1f;
        gprs[1] = gprs[1] & 0x1f;

        switch (opcode >> 26) {
        case MipsIVOpcodes::Addi:
            return [this, gprs, opcode]() {
                addi(opcode, mainMips.GPRAt<u32*>(gprs[0]), mainMips.GPRAt<u32*>(gprs[1]));
            };
        }
        return {};
    }
    u32 MipsIVInterpreter::fetchFromPc() {
        if (*mainMips.eePC & 4095)
            ;
        return mainMips.fetchByPC();
    }
}