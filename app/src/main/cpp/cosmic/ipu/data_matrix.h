#include <array>
namespace cosmic::ipu {
    template <typename T, u64 _Size>
    class DataMatrix {
    public:
        DataMatrix() {
        }
        std::span<T> operator[](const u64 idX) {
            return {&data[4 * idX], 4};
        }

    private:
        std::array<T, _Size * _Size> data;
    };
}