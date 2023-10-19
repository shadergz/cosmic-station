#pragma once

#include <span>

#include <sys/stat.h>
#include <unistd.h>
#include <android/log.h>

#include <fmt/format.h>

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

    class ZenFile {
        static constexpr auto invFile{-1};
        using FileStat = struct stat;
    public:
        ZenFile() : hld(-1) {}
        ZenFile(i32 fd) : hld(fd) {
            fstat(hld, &lastState);
        }
        ~ZenFile() {
            if (hld != invFile)
                close(hld);
        }
        void read(std::span<u8> here) {
            if (hld == invFile) {
                throw fatalError("Can't read from this fd (broken), error = {}", strerror(errno));
            }
            auto attempt{::read(hld, here.data(), here.size())};
            if (attempt == -1) {
                throw fatalError("Read operation failed with fd {} due to an error", hld);
            }
        }
        void operator=(int fdNative) {
            if (fdNative == invFile) {
                throw fatalError("Corrupted file descriptor being passed without checking");
            }
            hld = fdNative;
            fstat(hld, &lastState);
        }
        auto operator*()-> int {
            return hld;
        }
    private:
        FileStat lastState;
        int hld;
    };
}
