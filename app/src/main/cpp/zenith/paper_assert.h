#pragma once

#include <cassert>

#define PaperRtAssertPersistent(cond, assertMessage)\
    [[unlikely]] if (!(cond))\
        assert((cond))

#if defined(DEBUG)
#define PaperRtAssert(cond, assertMessage)\
    [[unlikely]] if (!(cond))\
        assert((cond))
#else
#define PaperRtAssert(cond, assertMessage)\
    (void)(cond)
#endif
