#ifndef RFL_INTERNAL_BIND_TO_TUPLE_HPP_
#define RFL_INTERNAL_BIND_TO_TUPLE_HPP_

// Adapted from the original in reflect-cpp
// (https://github.com/getml/reflect-cpp/blob/main/include/rfl/internal/bind_to_tuple.hpp)

#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

#include "always_false.hpp"
#include "num_fields.hpp"

namespace ThingSet {
namespace internal {

template <std::size_t n> struct tuple_view_helper
{
    template <class T> static constexpr auto tuple_view(T &)
    {
        static_assert(always_false_v<T>, "\n\nThis error occurs for one of two reasons:\n\n"
                                         "1) You have created a struct with more than 100 fields, which is "
                                         "unsupported. Please split up your struct into several "
                                         "smaller structs and then use rfl::Flatten<...> to combine them. "
                                         "Refer "
                                         "to the documentation on rfl::Flatten<...> for details.\n\n"
                                         "2) You have added a custom constructor to your struct, which you "
                                         "shouldn't do either. Please refer to the sections on custom "
                                         "classes or custom parsers in the documentation "
                                         "for solutions to this problem.\n\n");
    }
};

template <> struct tuple_view_helper<0>
{
    static constexpr auto tuple_view(auto &)
    {
        return std::tie();
    }
};

#define RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(n, ...)                         \
    template <> struct tuple_view_helper<n>                                                                            \
    {                                                                                                                  \
        static constexpr auto tuple_view(auto &t)                                                                      \
        {                                                                                                              \
            auto &[__VA_ARGS__] = t;                                                                                   \
            return std::tie(__VA_ARGS__);                                                                              \
        }                                                                                                              \
    }

/*The following boilerplate code was generated using a Python script:
macro =
"RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS"
with open("generated_code4.cpp", "w", encoding="utf-8") as codefile:
    codefile.write(
        "\n".join(
            [
                f"{macro}({i}, {', '.join([f'f{j}' for j in range(i)])});"
                for i in range(1, 101)
            ]
        )
    )
*/

RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(1, f0);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(2, f0, f1);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(3, f0, f1, f2);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(4, f0, f1, f2, f3);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(5, f0, f1, f2, f3, f4);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(6, f0, f1, f2, f3, f4, f5);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(7, f0, f1, f2, f3, f4, f5, f6);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(8, f0, f1, f2, f3, f4, f5, f6, f7);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(9, f0, f1, f2, f3, f4, f5, f6, f7, f8);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(10, f0, f1, f2, f3, f4, f5, f6, f7, f8,
                                                                               f9);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(11, f0, f1, f2, f3, f4, f5, f6, f7, f8,
                                                                               f9, f10);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(12, f0, f1, f2, f3, f4, f5, f6, f7, f8,
                                                                               f9, f10, f11);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(13, f0, f1, f2, f3, f4, f5, f6, f7, f8,
                                                                               f9, f10, f11, f12);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(14, f0, f1, f2, f3, f4, f5, f6, f7, f8,
                                                                               f9, f10, f11, f12, f13);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(15, f0, f1, f2, f3, f4, f5, f6, f7, f8,
                                                                               f9, f10, f11, f12, f13, f14);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(16, f0, f1, f2, f3, f4, f5, f6, f7, f8,
                                                                               f9, f10, f11, f12, f13, f14, f15);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(17, f0, f1, f2, f3, f4, f5, f6, f7, f8,
                                                                               f9, f10, f11, f12, f13, f14, f15, f16);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(18, f0, f1, f2, f3, f4, f5, f6, f7, f8,
                                                                               f9, f10, f11, f12, f13, f14, f15, f16,
                                                                               f17);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(19, f0, f1, f2, f3, f4, f5, f6, f7, f8,
                                                                               f9, f10, f11, f12, f13, f14, f15, f16,
                                                                               f17, f18);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(20, f0, f1, f2, f3, f4, f5, f6, f7, f8,
                                                                               f9, f10, f11, f12, f13, f14, f15, f16,
                                                                               f17, f18, f19);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(21, f0, f1, f2, f3, f4, f5, f6, f7, f8,
                                                                               f9, f10, f11, f12, f13, f14, f15, f16,
                                                                               f17, f18, f19, f20);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(22, f0, f1, f2, f3, f4, f5, f6, f7, f8,
                                                                               f9, f10, f11, f12, f13, f14, f15, f16,
                                                                               f17, f18, f19, f20, f21);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(23, f0, f1, f2, f3, f4, f5, f6, f7, f8,
                                                                               f9, f10, f11, f12, f13, f14, f15, f16,
                                                                               f17, f18, f19, f20, f21, f22);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(24, f0, f1, f2, f3, f4, f5, f6, f7, f8,
                                                                               f9, f10, f11, f12, f13, f14, f15, f16,
                                                                               f17, f18, f19, f20, f21, f22, f23);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(25, f0, f1, f2, f3, f4, f5, f6, f7, f8,
                                                                               f9, f10, f11, f12, f13, f14, f15, f16,
                                                                               f17, f18, f19, f20, f21, f22, f23, f24);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(26, f0, f1, f2, f3, f4, f5, f6, f7, f8,
                                                                               f9, f10, f11, f12, f13, f14, f15, f16,
                                                                               f17, f18, f19, f20, f21, f22, f23, f24,
                                                                               f25);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(27, f0, f1, f2, f3, f4, f5, f6, f7, f8,
                                                                               f9, f10, f11, f12, f13, f14, f15, f16,
                                                                               f17, f18, f19, f20, f21, f22, f23, f24,
                                                                               f25, f26);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(28, f0, f1, f2, f3, f4, f5, f6, f7, f8,
                                                                               f9, f10, f11, f12, f13, f14, f15, f16,
                                                                               f17, f18, f19, f20, f21, f22, f23, f24,
                                                                               f25, f26, f27);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(29, f0, f1, f2, f3, f4, f5, f6, f7, f8,
                                                                               f9, f10, f11, f12, f13, f14, f15, f16,
                                                                               f17, f18, f19, f20, f21, f22, f23, f24,
                                                                               f25, f26, f27, f28);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(30, f0, f1, f2, f3, f4, f5, f6, f7, f8,
                                                                               f9, f10, f11, f12, f13, f14, f15, f16,
                                                                               f17, f18, f19, f20, f21, f22, f23, f24,
                                                                               f25, f26, f27, f28, f29);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(31, f0, f1, f2, f3, f4, f5, f6, f7, f8,
                                                                               f9, f10, f11, f12, f13, f14, f15, f16,
                                                                               f17, f18, f19, f20, f21, f22, f23, f24,
                                                                               f25, f26, f27, f28, f29, f30);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(32, f0, f1, f2, f3, f4, f5, f6, f7, f8,
                                                                               f9, f10, f11, f12, f13, f14, f15, f16,
                                                                               f17, f18, f19, f20, f21, f22, f23, f24,
                                                                               f25, f26, f27, f28, f29, f30, f31);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(33, f0, f1, f2, f3, f4, f5, f6, f7, f8,
                                                                               f9, f10, f11, f12, f13, f14, f15, f16,
                                                                               f17, f18, f19, f20, f21, f22, f23, f24,
                                                                               f25, f26, f27, f28, f29, f30, f31, f32);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(34, f0, f1, f2, f3, f4, f5, f6, f7, f8,
                                                                               f9, f10, f11, f12, f13, f14, f15, f16,
                                                                               f17, f18, f19, f20, f21, f22, f23, f24,
                                                                               f25, f26, f27, f28, f29, f30, f31, f32,
                                                                               f33);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(35, f0, f1, f2, f3, f4, f5, f6, f7, f8,
                                                                               f9, f10, f11, f12, f13, f14, f15, f16,
                                                                               f17, f18, f19, f20, f21, f22, f23, f24,
                                                                               f25, f26, f27, f28, f29, f30, f31, f32,
                                                                               f33, f34);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(36, f0, f1, f2, f3, f4, f5, f6, f7, f8,
                                                                               f9, f10, f11, f12, f13, f14, f15, f16,
                                                                               f17, f18, f19, f20, f21, f22, f23, f24,
                                                                               f25, f26, f27, f28, f29, f30, f31, f32,
                                                                               f33, f34, f35);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(37, f0, f1, f2, f3, f4, f5, f6, f7, f8,
                                                                               f9, f10, f11, f12, f13, f14, f15, f16,
                                                                               f17, f18, f19, f20, f21, f22, f23, f24,
                                                                               f25, f26, f27, f28, f29, f30, f31, f32,
                                                                               f33, f34, f35, f36);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(38, f0, f1, f2, f3, f4, f5, f6, f7, f8,
                                                                               f9, f10, f11, f12, f13, f14, f15, f16,
                                                                               f17, f18, f19, f20, f21, f22, f23, f24,
                                                                               f25, f26, f27, f28, f29, f30, f31, f32,
                                                                               f33, f34, f35, f36, f37);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(39, f0, f1, f2, f3, f4, f5, f6, f7, f8,
                                                                               f9, f10, f11, f12, f13, f14, f15, f16,
                                                                               f17, f18, f19, f20, f21, f22, f23, f24,
                                                                               f25, f26, f27, f28, f29, f30, f31, f32,
                                                                               f33, f34, f35, f36, f37, f38);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(40, f0, f1, f2, f3, f4, f5, f6, f7, f8,
                                                                               f9, f10, f11, f12, f13, f14, f15, f16,
                                                                               f17, f18, f19, f20, f21, f22, f23, f24,
                                                                               f25, f26, f27, f28, f29, f30, f31, f32,
                                                                               f33, f34, f35, f36, f37, f38, f39);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(41, f0, f1, f2, f3, f4, f5, f6, f7, f8,
                                                                               f9, f10, f11, f12, f13, f14, f15, f16,
                                                                               f17, f18, f19, f20, f21, f22, f23, f24,
                                                                               f25, f26, f27, f28, f29, f30, f31, f32,
                                                                               f33, f34, f35, f36, f37, f38, f39, f40);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    42, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    43, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    44, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    45, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    46, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    47, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(48, f0, f1, f2, f3, f4, f5, f6, f7, f8,
                                                                               f9, f10, f11, f12, f13, f14, f15, f16,
                                                                               f17, f18, f19, f20, f21, f22, f23, f24,
                                                                               f25, f26, f27, f28, f29, f30, f31, f32,
                                                                               f33, f34, f35, f36, f37, f38, f39, f40,
                                                                               f41, f42, f43, f44, f45, f46, f47);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(49, f0, f1, f2, f3, f4, f5, f6, f7, f8,
                                                                               f9, f10, f11, f12, f13, f14, f15, f16,
                                                                               f17, f18, f19, f20, f21, f22, f23, f24,
                                                                               f25, f26, f27, f28, f29, f30, f31, f32,
                                                                               f33, f34, f35, f36, f37, f38, f39, f40,
                                                                               f41, f42, f43, f44, f45, f46, f47, f48);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    50, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    51, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    52, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    53, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    54, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    55, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    56, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54, f55);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    57, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54, f55, f56);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    58, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    59, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    60, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    61, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    62, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    63, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    64, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    65, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    66, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    67, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    68, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    69, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    70, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    71, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69,
    f70);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    72, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69,
    f70, f71);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    73, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69,
    f70, f71, f72);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    74, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69,
    f70, f71, f72, f73);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    75, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69,
    f70, f71, f72, f73, f74);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    76, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69,
    f70, f71, f72, f73, f74, f75);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    77, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69,
    f70, f71, f72, f73, f74, f75, f76);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    78, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69,
    f70, f71, f72, f73, f74, f75, f76, f77);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    79, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69,
    f70, f71, f72, f73, f74, f75, f76, f77, f78);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    80, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69,
    f70, f71, f72, f73, f74, f75, f76, f77, f78, f79);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    81, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69,
    f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    82, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69,
    f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    83, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69,
    f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    84, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69,
    f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82, f83);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    85, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69,
    f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82, f83, f84);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    86, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69,
    f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82, f83, f84, f85);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    87, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69,
    f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82, f83, f84, f85, f86);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    88, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69,
    f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82, f83, f84, f85, f86, f87);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    89, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69,
    f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82, f83, f84, f85, f86, f87, f88);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    90, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69,
    f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82, f83, f84, f85, f86, f87, f88, f89);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    91, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69,
    f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82, f83, f84, f85, f86, f87, f88, f89, f90);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    92, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69,
    f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82, f83, f84, f85, f86, f87, f88, f89, f90, f91);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    93, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69,
    f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82, f83, f84, f85, f86, f87, f88, f89, f90, f91, f92);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    94, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69,
    f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82, f83, f84, f85, f86, f87, f88, f89, f90, f91, f92,
    f93);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    95, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69,
    f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82, f83, f84, f85, f86, f87, f88, f89, f90, f91, f92,
    f93, f94);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    96, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69,
    f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82, f83, f84, f85, f86, f87, f88, f89, f90, f91, f92,
    f93, f94, f95);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    97, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69,
    f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82, f83, f84, f85, f86, f87, f88, f89, f90, f91, f92,
    f93, f94, f95, f96);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    98, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69,
    f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82, f83, f84, f85, f86, f87, f88, f89, f90, f91, f92,
    f93, f94, f95, f96, f97);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    99, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69,
    f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82, f83, f84, f85, f86, f87, f88, f89, f90, f91, f92,
    f93, f94, f95, f96, f97, f98);
RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS(
    100, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23,
    f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46,
    f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69,
    f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82, f83, f84, f85, f86, f87, f88, f89, f90, f91, f92,
    f93, f94, f95, f96, f97, f98, f99);

#undef RFL_INTERNAL_TUPLE_VIEW_IF_YOU_SEE_AN_ERROR_REFER_TO_DOCUMENTATION_ON_C_ARRAYS

template <class T> constexpr auto tuple_view(T &t)
{
    return tuple_view_helper<num_fields<T>>::tuple_view(t);
}

template <class T, typename F> constexpr auto bind_to_tuple(T &_t, const F &_f)
{
    auto view = tuple_view(_t);
    return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        return std::make_tuple(_f(std::get<Is>(view))...);
    }(std::make_index_sequence<std::tuple_size_v<decltype(view)>>());
}

} // namespace internal
} // namespace ThingSet

#endif
