#ifndef RFL_ALWAYSFALSE_HPP_
#define RFL_ALWAYSFALSE_HPP_

/// To be used inside visitor patterns
template <class> inline constexpr bool always_false_v = false;

#endif // RFL_ALWAYSFALSE_HPP_
