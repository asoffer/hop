#define JASMIN_INTERNAL_CONFIGURATION_DEBUG
#define JASMIN_INTERNAL_CONFIGURATION_HARDEN

#include "nth/meta/type.h"

namespace hop::internal {

template <typename T>
inline nth::type_id type_id = [] {
  if constexpr (std::is_pointer_v<T>) {
    return nth::type<void*>;
  } else {
    return nth::type<T>;
  }
}();

}  // namespace hop::internal
