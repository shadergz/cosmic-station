#pragma once

#include <creeper/ee/fast_macros.h>
#include <creeper/inst_operands.h>

#define RD_DW cpu->GPRs[ops.rd].dw[0]
#define RT_DW cpu->GPRs[ops.rt].dw[0]
#define RS_DW cpu->GPRs[ops.rs].dw[0]

#define RD_SW cpu->GPRs[ops.rd].sdw[0]
#define RT_SW cpu->GPRs[ops.rt].sdw[0]
#define RS_SW cpu->GPRs[ops.rs].sdw[0]

// #define RD_WORDS cpu->GPRs[ops.rd].words[0]
#define RT_WORDS cpu->GPRs[ops.rt].words[0]
#define RS_WORDS cpu->GPRs[ops.rs].words[0]

// #define RD_WORDS_S cpu->GPRs[ops.rd].swords[0]
#define RT_WORDS_S cpu->GPRs[ops.rt].swords[0]
#define RS_WORDS_S cpu->GPRs[ops.rs].swords[0]
