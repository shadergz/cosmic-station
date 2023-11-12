#pragma once

#define IvFuji3(op)\
    void op(i32 sfet, u32* gprDest, u32* gprSrc)

#define IvFujiSpecial(op)\
    void op(i32 sfet, u32* gprDest, u32* gprSrc, u32* gprExt)
