#pragma once

#include <thread>
namespace zenith::console {
    class EmuThread {
    public:
        EmuThread();
        void runVM();

    private:
        static void vmMain();
        std::thread vmt;
    };
}
