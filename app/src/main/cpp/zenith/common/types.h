#pragma once

#include <span>

#include <arm_neon.h>
#include <unistd.h>
#include <sys/stat.h>
#include <android/log.h>

#include <common/except.h>
namespace zenith {
    using u8 = std::uint8_t;
    using u16 = std::uint16_t;

    using i32 = std::int32_t;
    using i16 = std::int16_t;

    using u32 = std::uint32_t;

    using i64 = std::int64_t;
    using u64 = std::uint64_t;
    using f32 = float32_t;

    template<class To, class From>
        std::enable_if_t<sizeof(To) == sizeof(From) &&
            std::is_trivially_copyable_v<From> && std::is_trivially_copyable_v<To>, To>
    bit_cast(const From& src) noexcept {
        static_assert(std::is_trivially_constructible_v<To>, "This implementation additionally requires destination type to be trivially created");
        To dst;
        std::memcpy(&dst, &src, sizeof(To));
        return dst;
    }

    class ZenFile {
    public:
        static constexpr auto invFile{-1};
        using FileStat = struct stat;

        ZenFile() : hld(-1) {}
        ZenFile(i32 fd, bool isManaged = false)
            : hld(fd), closeAtDestroy(!isManaged) {
            if (fd != invFile)
                fstat(hld, &lastState);
        }
        ~ZenFile() {
            if (hld != invFile && closeAtDestroy)
                close(hld);
        }
        void operator=(i32 fdNative) {
            hld = fdNative;
            if (hld != invFile)
                fstat(hld, &lastState);
        }
        i32 getFd() const {
            return hld;
        }

        void read(std::span<u8> here) {
            if (hld == invFile)
                throw IOFail("Can't read from this fd (broken), error : {}", strerror(errno));

            auto attempt{::read(hld, here.data(), here.size())};
            if (attempt != here.size())
                throw IOFail("Read operation failed with fd {} due to an error", hld);
        }
        void readFrom(std::span<u8> here, u64 from) {
            lseek64(hld, bit_cast<off64_t>(from), SEEK_SET);
            read(here);
        }
    private:
        FileStat lastState{};
        i32 hld{invFile};
        bool closeAtDestroy{false};
    };
}
