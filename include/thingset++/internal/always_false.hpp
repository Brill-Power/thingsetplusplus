#ifndef RFL_ALWAYSFALSE_HPP_
#define RFL_ALWAYSFALSE_HPP_

// Adapted from the original in reflect-cpp
// (https://github.com/getml/reflect-cpp/blob/main/include/rfl/always_false.hpp)

namespace ThingSet {
namespace internal {

/// To be used inside visitor patterns
template <class> inline constexpr bool always_false_v = false;

} // namespace internal
} // namespace ThingSet

#endif // RFL_ALWAYSFALSE_HPP_
