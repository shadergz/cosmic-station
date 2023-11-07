#pragma once
#include <map>

#include <eeiv/ee_handler.h>
namespace zenith::fuji {
    template <typename T>
    class raw_reference {
    public:
        raw_reference() = default;
        raw_reference(T& save) {
            safeRaw.reset();
            safeRaw = save;
        }
        auto operator->() {
            return &(safeRaw.value().get());
        }
        std::optional<std::reference_wrapper<T>> safeRaw;
    };

    enum MipsIVOpcodes {
        Addi = 0x8
    };

    struct CachedBlock {
        u16 trackIndex;
        u32 trackablePC;
        std::function<void()> execute;

        raw_reference<CachedBlock> nextBlock;
    };

    class MipsIVInterpreter : public eeiv::EEExecutor {
    public:
        static constexpr u16 superBlockCount{256};
        MipsIVInterpreter(eeiv::EEMipsCore& mips);
        u32 executeCode() override;
    private:
        void runBlocks(u32 pc, CachedBlock& block);
        u32 runByCounter(u32 counter, CachedBlock& block);
        u32 runNestedBlocks(CachedBlock& block);
        void feedEntireCache(u32 nextPC);

        u32 fetchFromPc();
        std::function<void()> decodeFunc(u32 opcode);

        std::map<u32, CachedBlock> cached;

        void addi(u32 fetched, u32* gprDest, u32* gprSrc);
    };
}
