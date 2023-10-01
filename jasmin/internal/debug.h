#ifndef JASMIN_INTERNAL_DEBUG_H
#define JASMIN_INTERNAL_DEBUG_H
#if defined(JASMIN_DEBUG)

#include <concepts>
#include <cstdio>
#include <cstdlib>
#include <typeinfo>
#include <type_traits>

namespace jasmin::internal {

// A type-id mechanism which treats all pointers as equivalent. Strictly
// speaking, we would prefer to test compatibility, but this is generally not
// possible via this mechanism.
struct TypeId {
  void const *value = nullptr;
  char const *name  = "unknown";

  friend bool operator==(TypeId const &lhs, TypeId const &rhs) {
    return lhs.value == rhs.value;
  }
  friend bool operator!=(TypeId const &lhs, TypeId const &rhs) {
    return not(lhs == rhs);
  }

  friend void NthPrint(auto &p, auto &, TypeId const &t) { p.write(t.name); }
};

template <typename T>
inline TypeId type_id{
    .value = &type_id<std::conditional_t<std::is_pointer_v<T>, void *, T>>,
    .name  = typeid(T).name(),
};

}  // namespace jasmin::internal

#endif  // defined(JASMIN_DEBUG)
#endif  // JASMIN_INTERNAL_DEBUG_H
