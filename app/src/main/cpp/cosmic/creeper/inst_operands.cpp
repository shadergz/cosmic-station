#include <range/v3/algorithm.hpp>
#include <creeper/inst_operands.h>
#include <engine/ee_info.h>

namespace cosmic::creeper {
    using EeCore = engine::MipsIvOpcodes;

    std::array<const char*, 3> EeOpcodeTranslator::interpreters{
        "Emotion Engine by Cached Interpreter",
        "IOP Interpreter",
        "VU (0, 1) Interpreter"
    };
    OpcodeMapType EeOpcodeTranslator::eeMipsCoreFmt{
        {EeCore::Ori, {"{} {},{},{}"}},
    };

    std::array<const char*, 1> EeOpcodeTranslator::eeOps{""};
    std::array<const char*, 1> EeOpcodeTranslator::iopOps{""};
    std::array<const char*, 1> EeOpcodeTranslator::vuOps{""};
}
