#ifndef RFL_ALWAYSFALSE_HPP_
#define RFL_ALWAYSFALSE_HPP_

namespace ThingSet {
namespace internal {

/// To be used inside visitor patterns
template <class> inline constexpr bool always_false_v = false;

} // namespace internal
} // namespace ThingSet

#endif // RFL_ALWAYSFALSE_HPP_
