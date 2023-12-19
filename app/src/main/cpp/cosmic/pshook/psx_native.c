#include <stdint.h>
#include <string.h>
#include <stdlib.h>
typedef int32_t i32;
typedef uint32_t u32;

union R { i32 s; u32 u; };

struct PsxRegs {
    union R* a0;
    union R* v0;
    union R* pc;
    union R* ra;
};
struct PsxRegs r;

void psxAbs() {
    r.v0->s = abs(r.a0->s);
    *r.pc = *r.ra;
}
