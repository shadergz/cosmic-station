#pragma once
#include <translator/inst_operands.h>
#include <vu/vu_info.h>
namespace cosmic::translator::micro {
    struct VuMicroOrder {
        // A VU, in microcode mode, executes more than one instruction,
        // but there may be a dependency between registers
        std::function<void(VuMicroOperands&)> upper, lower;
        struct VecAffected {
            std::array<u8, 2> read0;
            std::array<u8, 2> read0Field;
            std::array<u8, 2> read1;
            std::array<u8, 2> read1Field;
            std::array<u8, 2> write;
            std::array<u8, 2> writeField;
        } fr;
        struct IntAffected {
            u8 write;
            u8 read0;
            u8 read1;
        } ir;
    };

    class [[maybe_unused]] VuMicroInterpreter : vu::VuMicroExecutor {
        u32 executeCode() override;
        void setCurrentProgram(u32 crc) override;
        void startProgram() override;
        void stopProgram() override;

        u32 fetchPcInst(u32 pc) override;
        VuMicroOperands translateUpper(u32 upper);
        VuMicroOperands translateLower(u32 lower);
        VuMicroOperands translateLower1(u32 lower);
        VuMicroOperands translateLower2(u32 lower);

        void waitp(VuMicroOperands& ops); void waitq(VuMicroOperands& ops);
        void maxi(VuMicroOperands& ops);

        void iddai(VuMicroOperands& ops);
    private:
        VuMicroOrder ordered;
    };
}
