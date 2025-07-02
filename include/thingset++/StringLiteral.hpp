#pragma once

#include <algorithm>
#include <array>
#include <string>
#include <string_view>

// Adapted from the original in reflect-cpp
// (https://github.com/getml/reflect-cpp/blob/main/include/rfl/internal/StringLiteral.hpp)

namespace ThingSet {

// https://stackoverflow.com/questions/6713420/c-convert-integer-to-string-at-compile-time
template<std::intmax_t N>
class to_string_t {
public:
    constexpr static auto size() noexcept {
        unsigned int len = N > 0 ? 1 : 2;
        for (auto n = N; n; len++, n /= 10);
        return len;
    }

private:
    char buf[size()] = {};

public:
    constexpr to_string_t() noexcept {
        auto ptr = buf + size();
        *--ptr = '\0';

        if (N != 0) {
            for (auto n = N; n; n /= 10)
                *--ptr = "0123456789"[(N < 0 ? -1 : 1) * (n % 10)];
            if (N < 0)
                *--ptr = '-';
        } else {
            buf[0] = '0';
        }
    }

    constexpr operator const char *() const { return buf; }
};

/// Normal strings cannot be used as template
/// parameters, but this can. This is needed
/// for the parameters names in the NamedTuples.
template <size_t N> struct StringLiteral
{
    constexpr StringLiteral(const auto... _chars) : arr_{ _chars..., '\0' }
    {}

    constexpr StringLiteral(const std::array<char, N> _arr) : arr_(_arr)
    {}

    constexpr StringLiteral(const char (&_str)[N])
    {
        std::copy_n(_str, N, std::data(arr_));
    }

    /// Returns the value as a string.
    std::string str() const
    {
        return std::string(std::data(arr_), N - 1);
    }

    /// Returns the value as a string.
    constexpr std::string_view string_view() const
    {
        return std::string_view(std::data(arr_), N - 1);
    }

    std::array<char, N> arr_{};
};

template <size_t N1, size_t N2>
constexpr inline StringLiteral<N1 + N2> operator,(const StringLiteral<N1> &left, const StringLiteral<N2> &right)
{
    std::array<char, N1 + N2> array;
    std::copy_n(&left.arr_[0], N1, std::data(array));
    array[N1 - 1] = ',';
    std::copy_n(&right.arr_[0], N2, &(std::data(array))[N1]);
    return StringLiteral(array);
}

template <size_t N1, typename T>
constexpr inline StringLiteral<N1 + T::size() - 1> operator+(const StringLiteral<N1> &left, const T& right)
{
    std::array<char, N1 + T::size() - 1> array;
    std::copy_n(&left.arr_[0], N1, std::data(array));
    const char* rhs = right;
    std::copy_n(rhs, T::size(), &(std::data(array))[N1 - 1]);
    return StringLiteral(array);
}

template <size_t N1, size_t N2>
constexpr inline StringLiteral<N1 + N2 - 1> operator+(const StringLiteral<N1> &left, const char (&right)[N2])
{
    std::array<char, N1 + N2 - 1> array;
    std::copy_n(&left.arr_[0], N1, std::data(array));
    std::copy_n(right, N2, &(std::data(array))[N1 - 1]);
    return StringLiteral(array);
}

template <size_t N1, size_t N2>
constexpr inline StringLiteral<N1 + N2 - 1> operator+(const char (&left)[N1], const StringLiteral<N2> &right)
{
    std::array<char, N1 + N2 - 1> array;
    std::copy_n(left, N1, std::data(array));
    std::copy_n(&right.arr_[0], N2, &(std::data(array))[N1 - 1]);
    return StringLiteral(array);
}

template <size_t N1, size_t N2>
constexpr inline StringLiteral<N1 + N2 - 1> operator+(const StringLiteral<N1> &left, const StringLiteral<N2> &right)
{
    std::array<char, N1 + N2 - 1> array;
    std::copy_n(&left.arr_[0], N1, std::data(array));
    std::copy_n(&right.arr_[0], N2, &(std::data(array))[N1 - 1]);
    return StringLiteral(array);
}

template <size_t N1, size_t N2>
constexpr inline bool operator==(const StringLiteral<N1> &_first, const StringLiteral<N2> &_second)
{
    if constexpr (N1 != N2) {
        return false;
    }
    return _first.string_view() == _second.string_view();
}

template <size_t N1, size_t N2>
constexpr inline bool operator!=(const StringLiteral<N1> &_first, const StringLiteral<N2> &_second)
{
    return !(_first == _second);
};
} // namespace ThingSet