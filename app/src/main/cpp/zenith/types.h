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
    using sz64 = std::size_t;
    static_assert(sizeof(u64) == sizeof(sz64), "");

    template<class To, class From>
        std::enable_if_t<sizeof(To) == sizeof(From) &&
            std::is_trivially_copyable_v<From> && std::is_trivially_copyable_v<To>, To>
    bit_cast(const From& src) noexcept {
        static_assert(std::is_trivially_constructible_v<To>, "This implementation additionally requires destination type to be trivially created");
        To dst;
        std::memcpy(&dst, &src, sizeof(To));
        return dst;
    }

    class fatalError : public std::runtime_error {
    public:
        template <typename T, typename... Args>
        fatalError(const T& format, Args&&... args)
            : std::runtime_error(fmt::format(fmt::runtime(format), args...)) {}
    };

    class ZenFile {
        static constexpr auto invFile{-1};
    public:
        using FileStat = struct stat;

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
                throw fatalError("Can't read from this fd (broken), error : {}", strerror(errno));
            }
            auto attempt{::read(hld, here.data(), here.size())};
            if (attempt != here.size()) {
                throw fatalError("Read operation failed with fd {} due to an error", hld);
            }
        }
        void readFrom(std::span<u8> here, u64 from) {
            lseek64(hld, bit_cast<off64_t>(from), SEEK_SET);
            read(here);
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
