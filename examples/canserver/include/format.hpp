#ifndef __cpp_lib_format
// std::format polyfill using fmtlib
#include <fmt/core.h>
namespace std {
using fmt::format;
}
#else
#include <format>
#endif