#pragma once

#include <span>
#include <optional>

#include <sys/stat.h>
#include <android/log.h>

#include <common/except.h>
#include <common/alias.h>
namespace zenith {
    template <typename T>
    class raw_reference {
    public:
        raw_reference() = default;
        raw_reference(T& save) {
            safeRaw = save;
        }
        auto operator=(std::reference_wrapper<T>&& wrapper) {
            safeRaw = wrapper;
            return *this;
        }
        auto operator->() {
            return &(safeRaw.value().get());
        }
        explicit operator bool() const {
            return safeRaw.has_value();
        }
        std::optional<std::reference_wrapper<T>> safeRaw;
    };

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
