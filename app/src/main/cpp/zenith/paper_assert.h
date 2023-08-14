#pragma once

#include <cassert>

#define PaperRtAssertPersistent(leftOp, rightOp, assertMessage)\
    [[unlikely]] if ((leftOp) != (rightOp))\
        assert((leftOp) == (rightOp))
