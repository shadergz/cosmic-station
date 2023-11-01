#pragma once

#include <thread>
namespace zenith::console {
    class EmuThread {
    protected:
        EmuThread();
        void runVM();

    private:
        static void vmMain();
        std::thread vmt;
    };
}
