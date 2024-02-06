#include <array>
#include <range/v3/algorithm.hpp>
namespace cosmic::ipu {
    template <typename T, u64 _Size>
    class DataMatrix {
    public:
        DataMatrix() {
        }
        std::span<T> operator[](const u64 idX) {
            return {&data[4 * idX], 4};
        }

        void clear() {
            ranges::fill(data, T{});
        }

    private:
        std::array<T, _Size * _Size> data;
    };
}