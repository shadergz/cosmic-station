#pragma once

#include <cassert>

#define PaperRtAssertPersistent(cond, assertMessage)\
    [[unlikely]] if (!(cond))\
        assert((cond))
