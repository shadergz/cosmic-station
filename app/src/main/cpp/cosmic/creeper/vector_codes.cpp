#include <creeper/vector_codes.h>
#include <vu/vecu.h>
namespace cosmic::creeper {
    Wrapper<vu::VectorUnit> VuMicroInterpreter::vu;

    u32 VuMicroInterpreter::executeCode() {
        VuMicroOperands ops[2];
        const auto [upper, lower] = fetchPcInst();

        u32 micro[2];
        micro[0] = upper;
        micro[1] = lower;
        // Check if we need to interlock the Q register until a previous FDIV instruction calculation result is finished
        // FDIV, SQRT
        if (((micro[1] & 0x800007fc) == 0x800003bc))
            waitq(ops[1]);
        if ((micro[1] & (static_cast<u32>(1 << 31))) && ((micro[1] >> 2) & 0x1fc) == 0x1fc)
            waitp(ops[1]);

        ops[0] = translateUpper(micro[0]);
        bool isUpper{true};
        if (!(micro[0] & static_cast<u32>(1 << 31))) {
            ops[1] = translateLower(micro[1]);
            isUpper = {};
        }

        if (isUpper) {
            // Upper-type opcodes always execute first
            ordered.upper(ops[1]);
            // Saving the Lower-type instruction in the special instruction register
            vuMicro->setSpecialReg(vu::I, micro[1]);
        } else {
            std::array<u32, 2> upperRegs{
                ordered.fr.writeOpcodes[0],
                // Dest Register
                ordered.fr.readUpper[1]
            };
            std::array<u32, 2> lowerRegs{
                ordered.fr.writeOpcodes[1],
                // Source register
                ordered.fr.readLower[1],
            };
            const auto regAffectedWrite{upperRegs[0] == lowerRegs[0]};
            // Check if the upper instruction will affect the register that the lower
            // instruction is using as a source
            const auto lowerIsAffected{upperRegs[0] == lowerRegs[1]};
            if (!regAffectedWrite && lowerIsAffected) {
                ordered.lower(ops[0]);
                ordered.upper(ops[1]);
            }
            ordered.upper(ops[1]);
            ordered.lower(ops[0]);
        }
        return {};
    }

    void VuMicroInterpreter::setCurrentProgram(u32 crc) {
    }

    std::pair<u32, u32> VuMicroInterpreter::fetchPcInst() {
        u32 u, l;
        l = vu->fetchByPc();
        u = vu->fetchByPc();
        return std::make_pair(u, l);
    }
    void VuMicroInterpreter::waitp(VuMicroOperands& ops) {
        if (!vu->status.efu.isStarted) {
            return;
        }
        vu->finishStallPipeTask(false);
    }
    void VuMicroInterpreter::waitq(VuMicroOperands& ops) {
        auto& div{vu->status.div};
        if (!div.isStarted)
            return;
        vu->finishStallPipeTask(true);
    }

    VuMicroOperands VuMicroInterpreter::translateUpper(u32 upper) {
        u8 decMi[1];
        decMi[0] = upper & 0x3f;
        VuMicroOperands ops{upper};
        ordered.fr.writeOpcodes[0] = ops.fd; // DEST
        ordered.fr.writeOpcodesField[0] = ops.field; // DEST FIELD
        ordered.fr.readUpperField[0] = ordered.fr.writeOpcodesField[0]; // SOURCE FIELD

        ordered.fr.readUpper[0] = ops.fs; // SOURCE

        ordered.fr.readLower[0] = upper & 0x3; // BACKUP AFFECTED
        ordered.fr.readLowerField[0] = static_cast<u8>(1 << (0x3 - (ordered.fr.readLower[0])));

        switch (decMi[0]) {
        case 0x1d: ordered.upper = maxi; break;
        }
        return ops;
    }
    VuMicroOperands VuMicroInterpreter::translateLower(u32 lower) {
        if (lower & (static_cast<u32>(1 << 31))) {
            return translateLower1(lower);
        } else {
            return translateLower2(lower);
        }
    }

    void VuMicroInterpreter::maxi(VuMicroOperands& ops) {
    }
    VuMicroOperands VuMicroInterpreter::translateLower1(u32 lower) {
        VuMicroOperands intOps{lower};
        ordered.fr.writeOpcodes[1] = intOps.fd;
        ordered.fr.writeOpcodesField[1] = intOps.field;

        ordered.fr.readUpper[1] = intOps.fs;

        ordered.fr.readUpperField[1] = static_cast<u8>(
            (intOps.field >> 1) | ((intOps.field & 0xf) << 3));
        std::array<u32, 2> d2opc{lower & 0x3f, 0};

        switch (d2opc[0]) {
        case 0x32: ordered.lower = iddai; break;
        case 0x3f:
            d2opc[1] = (lower & 0x3) | ((lower >> 4) & 0x7c);
            switch (d2opc[1]) {
            case 0x31: ordered.lower = mr32; break;
            case 0x3c: ordered.lower = mtir; break;
            }
        }
        return intOps;
    }
    VuMicroOperands VuMicroInterpreter::translateLower2(u32 lower) {
        switch ((lower >> 25) & 0x7f) {
        }
        return {};
    }
}
