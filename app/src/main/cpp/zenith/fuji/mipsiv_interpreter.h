#pragma once
#include <vector>

#include <eeiv/ee_handler.h>
#include <common/types.h>

#define IvFuji3(op)\
    void op(i32 sfet, u32* gprDest, u32* gprSrc)

#define IvFujiSpecial(op)\
    void op(i32 sfet, u32* gprDest, u32* gprSrc, u32* gprExt)

#define IvFuji3Impl(op) IvFuji3(MipsIVInterpreter::op)
#define IvFujiSpecialImpl(op) IvFujiSpecial(MipsIVInterpreter::op)

namespace zenith::fuji {
    enum MipsIVOpcodes : u16 {
        SpecialOpcodes = 0x0,
        RegImmOpcodes = 0x1,
        CopOp2Tlbr = 0x1,
        Addi = 0x8,
        Slti = 0xa,
        RegImmBltzal = 0x10,
        CopOpcodes = 0x10,
        CopOp2 = 0x010,
        CopOp2Eret = 0x18,
        CopOp2Ei = 0x38,
        CopOp2Di = 0x39,
        Lb = 0x20,
        Lh = 0x21,
        Lw = 0x23,
        Lbu = 0x24,
        Lhu = 0x25,
        Lwu = 0x27,
        Cache = 0x2f,
        Nop = 0x33,
        Ld = 0x37,

        SpecialSlt = 0x2a,
        Sw = 0x2b,
        SpecialXor = 0x26,
    };

    struct CachedBlock {
        u16 trackIndex;
        u32 trackablePC;
        std::function<void()> execute;
    };

    class MipsIVInterpreter : public eeiv::EEExecutor {
    public:
        static constexpr u16 superBlockCount{0xff};
        MipsIVInterpreter(eeiv::EEMipsCore& mips);
        u32 executeCode() override;
    private:
        void runBlocks(u32 pc, u32 block);
        u32 runByCounter(u32 counter, u32 block);
        u32 runNestedBlocks(u32 block);
        void feedEntireCache(u32 nextPC);

        u32 fetchFromPc();
        std::function<void()> decodeFunc(u32 opcode);
        void performOp(std::function<void()> func);

        std::vector<CachedBlock> cached;

        IvFuji3(addi);
        IvFuji3(slti);

        IvFuji3(sw); IvFuji3(sd);

        IvFuji3(bltzal);

        // Memory read functions through direct translation
        IvFuji3(lb); IvFuji3(lbu);
        IvFuji3(lh); IvFuji3(lhu);
        IvFuji3(lw); IvFuji3(lwu);
        IvFuji3(ld);

        IvFuji3(cache);
        IvFuji3(nop);

        IvFujiSpecial(slt);
        IvFujiSpecial(ivXor);

        // Instructions intrinsically related to C0 and TLB/Exception
        IvFuji3(tlbr);
        IvFuji3(eret);
        IvFuji3(ei);
        IvFuji3(di);
    };
}
