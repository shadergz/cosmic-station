#pragma once

#include <span>
#include <optional>

#include <sys/stat.h>
#include <android/log.h>

#include <common/except.h>
#include <common/alias.h>
namespace cosmic {
    template <typename T>
    class Wrapper : public std::optional<std::reference_wrapper<T>> {
    public:
        Wrapper() = default;
        Wrapper(T& reference) : std::optional<std::reference_wrapper<T>>(reference) {
        }
        auto take() const {
            return this->value().get();
        }
        auto take() {
            return std::addressof(this->value().get());
        }
        auto operator->() {
            return &(this->value().get());
        }
    };

    template<class To, class From>
        std::enable_if_t<sizeof(To) == sizeof(From) &&
            std::is_trivially_copyable_v<From> && std::is_trivially_copyable_v<To>, To>
    BitCast(const From& src) noexcept {
        static_assert(std::is_trivially_constructible_v<To>, "This implementation additionally requires destination type to be trivially created");
        To dst;
        std::memcpy(&dst, &src, sizeof(To));
        return dst;
    }

    class DescriptorRaii {
    public:
        static constexpr auto invFile{-1};
        using FileStat = struct stat;

        DescriptorRaii() : hld(-1) {}
        DescriptorRaii(i32 fd, bool isManaged = false) :
            hld(fd), closeAtDestroy(!isManaged) {
            if (fd != invFile)
                fstat(hld, &lastState);
        }
        ~DescriptorRaii() {
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
                throw IoErr("Can't read from this fd (broken), error: {}", strerror(errno));

            auto attempt{::read(hld, here.data(), here.size())};
            if (attempt != here.size())
                throw IoErr("Read operation failed with fd {} due to an error", hld);
        }
        void readFrom(std::span<u8> here, u64 from) {
            lseek64(hld, BitCast<off64_t>(from), SEEK_SET);
            read(here);
        }
    private:
        FileStat lastState{};
        i32 hld{invFile};
        bool closeAtDestroy{false};
    };
}
