#pragma once

#include <sys/stat.h>

namespace zenith {

    struct BasicFile {
        using FileStat = struct stat;
    public:
        [[maybe_unused]] FileStat lastStates;
        int basicFd;

        auto operator*()-> int {
            return basicFd;
        }

    };
}
