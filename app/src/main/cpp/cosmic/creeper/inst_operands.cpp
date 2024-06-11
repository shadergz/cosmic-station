#include <range/v3/all.hpp>

#include <creeper/inst_operands.h>
#include <engine/ee_info.h>

#include <string_view>
namespace cosmic::creeper {
    using EeCore = engine::MipsIvOpcodes;

    std::array<std::string, 3> EeOpcodeTranslator::interpreters{
        "Emotion Engine by Cached Interpreter",
        "IOP Interpreter",
        "VU (0, 1) Interpreter"
    };
    OpcodeMapType EeOpcodeTranslator::eeMipsCoreFmt{
        {EeCore::Ori, {"{} {},{},{}"}},
    };

    std::array<std::string, 1> EeOpcodeTranslator::eeOps{""};
    std::array<std::string, 1> EeOpcodeTranslator::iopOps{""};
    std::array<std::string, 1> EeOpcodeTranslator::vuOps{""};

    GenericDisassembler::GenericDisassembler(std::span<const std::string> alias, u32 pc, ConvMode ctrl) :
        registersAlias(alias),
        currPc(pc),
        dis(ctrl),
        ops() {
    }

    class HashString {
        using HashResult = u64;
    public:
        HashString() = default;
        constexpr auto operator()(std::string_view view) -> HashResult {
            if (view.size() > 4)
                return view[0] + view[1] + view[2] + view[3];
            if (view.size() < 2)
                return {};
            return (view[0] * 5) + (view[1] / 5) + (view[2] * 10);
        }
    };
    static HashString ConstHash{};
    void GenericDisassembler::mips2Str(std::string& output,
        std::string_view& format) {
        const u32 offsetFromOpc{ops.inst & 0xffff};
        std::string temp{};

        while (format.size() > 0) {
            bool isOffset{};
            u32 offset{};
            auto next{format.find_first_of(", ", 3)};
            auto view{format.substr(1, next - 1)};

            switch (ConstHash(view)) {
            case ConstHash("$RD"):
                offset = ops.rd; break;
            case ConstHash("$RT"):
                offset = ops.rt; break;
            case ConstHash("$RS"):
            case ConstHash("$BASE"):
                offset = ops.rs; break;
            case ConstHash("$OFF"):
            case ConstHash("$IMM"):
                offset = offsetFromOpc;
                if (ConstHash(format) == ConstHash("$OFF"))
                    offset = currPc + (offsetFromOpc << 2);
                isOffset = true;
            }

            if (!isOffset) {
                auto& alias{registersAlias[offset]};
                fmt::format_to(std::back_inserter(temp),"${}", alias);
            } else {
                fmt::format_to(std::back_inserter(temp),"#{:#x}", offset);
            }

            if (!output.ends_with(' '))
                output += ", " + temp;
            else
                output += " " + temp;
            temp.clear();

            if (next == static_cast<u64>(-1))
                break;
            format.remove_prefix(next + 1);
        }
    }

    void GenericDisassembler::convMicro2Str(Operands& opsList,
        std::string& output, std::string_view& format) {
        this->ops = opsList;

        if (dis == ConvMode::Mips && format.size()) {
            mips2Str(output, format);
        }
    }
}
