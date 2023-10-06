#pragma once

#include <sys/stat.h>
#include <unistd.h>

#include <fmt/format.h>

#include <verify.h>
namespace zenith {
    using u8 = std::uint8_t;
    using u16 = std::uint16_t;

    using i32 = std::int32_t;
    using i16 = std::int16_t;

    using u32 = std::uint32_t;

    using u64 = std::uint64_t;

    class fatalError : public std::runtime_error {
    public:
        template <typename T, typename... Args>
        fatalError(const T& format, Args&&... args)
            : std::runtime_error(fmt::format(fmt::runtime(format), args...)) {}
    };

    struct ZenFile {
        static constexpr auto invalidFileDescriptor{-1};
        using FileStat = struct stat;
    public:
        ZenFile()
            : basicFd(-1) {}

        ~ZenFile() {
            if (basicFd != invalidFileDescriptor)
                close(basicFd);
        }
        FileStat lastStates;
        int basicFd;

        void operator=(int fileNativeFd) {
            if (fileNativeFd == invalidFileDescriptor) {
                throw fatalError("Corrupted file descriptor being passed without checking");
            }
            basicFd = fileNativeFd;

            fstat(basicFd, &lastStates);
            VerifyRtAssert((lastStates.st_mode & S_IFMT) == S_IFREG);
        }
        auto operator*()-> int {
            return basicFd;
        }
    };
}
