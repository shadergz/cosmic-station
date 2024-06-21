#pragma once
#include <creeper/inst_operands.h>
#include <vu/vu_info.h>
namespace cosmic::creeper {
    struct VuMicroOrder {
        // A VU, in microcode mode, executes more than one instruction,
        // but there may be a dependency between registers
        std::function<void(VuMicroOperands&)> upper, lower;
        struct VecAffected {

            std::array<u8, 2> writeOpcodes;
            std::array<u8, 2> writeOpcodesField;

            // Both types can consume more than one register in a single instruction
            std::array<u8, 2> readUpper;
            std::array<u8, 2> readUpperField;
            std::array<u8, 2> readLower;
            std::array<u8, 2> readLowerField;
        } fr;
        struct IntAffected {
            u8 writeOpcodes;
            u8 readUpperOpcodes;
            u8 readLowerOpcodes;
        } ir;
    };

    class VuMicroInterpreter : public vu::VuMicroExecutor {
    public:
        VuMicroInterpreter(Optional<vu::VectorUnit> vuCake) :
            vu::VuMicroExecutor(vuCake) {
            vu = vuMicro;
        }
        u32 executeCode() override;
        void setCurrentProgram(u32 crc) override;

        std::pair<u32, u32> fetchPcInst() override;
        VuMicroOperands translateUpper(u32 upper);
        VuMicroOperands translateLower(u32 lower);
        VuMicroOperands translateLower1(u32 lower);
        VuMicroOperands translateLower2(u32 lower);

        static void waitp(VuMicroOperands& ops);
        static void waitq(VuMicroOperands& ops);
        static void maxi(VuMicroOperands& ops);

        static void iddai(VuMicroOperands& ops);
        static void mtir(VuMicroOperands& ops);
        static void mr32(VuMicroOperands& ops);
        static void mul(VuMicroOperands& ops);
        static void mula(VuMicroOperands& ops);
        static void mulabc(VuMicroOperands& ops);
        static void mulai(VuMicroOperands& ops);
        static void mulaq(VuMicroOperands& ops);

        static f32 fasterPs2VuMul(Reg rad, u32 id, Reg mul, u32 idx);
        static void vectorizeXyZw(VuMicroOperands& ops, std::function<void(u32 xYzW)> vecMathCb);
    private:
        VuMicroOrder ordered;

        static Optional<vu::VectorUnit> vu;
    };
}
