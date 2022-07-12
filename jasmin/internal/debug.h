#ifndef JASMIN_DEBUG_H
#define JASMIN_DEBUG_H
#if defined(JASMIN_DEBUG)

#include <cstdio>
#include <cstdlib>
#include <type_traits>

namespace jasmin::internal_debug {

// A type-id mechanism which treats all pointers as equivalent. Strictly
// speakiung, we would prefer to test compatibility, but this is generally not
// possible via this mechanism.
template <typename T>
inline constexpr void const *type_id =
    &type_id<std::conditional_t<std::is_pointer_v<T>, void *, T>>;

// Writes the error message `message` to `stderr` and aborts execution.
//
// TODO: Bazel builds on MacOS are unable to create archive files due to
// specifying a non-existant -D flag. This funciton does not need to be inline,
// and should not be inline as soon as this issue is resolved.
[[noreturn]] inline void DebugAbort(char const *failed, char const *message) {
  std::fputs(failed, stderr);
  std::fputs(message, stderr);
  std::abort();
}

}  // namespace jasmin::internal_debug

#define JASMIN_INTERNAL_DEBUG_ASSERT(expr, message)                            \
  do {                                                                         \
    if (not(expr)) {                                                           \
      ::jasmin::internal_debug::DebugAbort("Failed assertion: " #expr "\n",    \
                                           message);                           \
    }                                                                          \
  } while (false)

#else

#define JASMIN_INTERNAL_DEBUG_ASSERT(expr, message)

#endif  // defined(JASMIN_DEBUG)
#endif  // JASMIN_DEBUG_H
