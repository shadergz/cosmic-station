#include <gs/gif_mandatory.h>
#include <gs/synth_engine.h>

namespace cosmic::gs {
    void gifQueueReset();
    u8 gifQueueSize();

    bool GifArk::downloadGsData(os::vec& put) {
        auto gsResult{gs->readGsData()};
        if (std::get<0>(gsResult))
            put = std::get<1>(gsResult);
        return std::get<0>(gsResult);
    }
    void GifArk::resumeDmacPath() {
        if (status.path3enbGif || status.path3enbVifMask)
            return;
        if (paths[3].status != Available)
            return;
        if (!gifQueueSize())
            return;
        paths[3].status = Busy;
        reqADmacAtPath(0x3);
    }
    void GifArk::reqADmacAtPath(u8 path, bool intPath3) {
    }

    void GifArk::resetGif() {
        for (u8 path{}; path < 3; path++) {
            paths[path].tag = GifTag{};
            paths[path].status = Available;
        }
        activatePath = 0;
        status = {};
    }
}
