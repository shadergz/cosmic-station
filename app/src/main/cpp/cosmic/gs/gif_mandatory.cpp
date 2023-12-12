#include <gs/gif_mandatory.h>
#include <gs/synth_engine.h>

namespace cosmic::gs {
    bool GifArk::downloadGsData(os::vec128& put) {
        auto gsResult{gs->readGsData()};
        if (std::get<0>(gsResult))
            put = std::get<1>(gsResult);
        return std::get<0>(gsResult);
    }
}
