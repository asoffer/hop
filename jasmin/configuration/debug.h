#define JASMIN_INTERNAL_CONFIGURATION_DEBUG
#define JASMIN_INTERNAL_CONFIGURATION_HARDEN

#include "nth/meta/type.h"

namespace jasmin::internal {

template <typename T>
inline nth::TypeId type_id = [] {
  if constexpr (std::is_pointer_v<T>) {
    return nth::type<void*>;
  } else {
    return nth::type<T>;
  }
}();

}  // namespace jasmin::internal
