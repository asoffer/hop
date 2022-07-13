#include "jasmin/internal/debug.h"

#if defined(JASMIN_DEBUG)

#include <cstdio>
#include <cstdlib>

namespace jasmin::internal_debug {

[[noreturn]] void DebugAbort(char const *failed, char const *message) {
  std::fputs(failed, stderr);
  std::fputs(message, stderr);
  std::abort();
}

}  // namespace jasmin::internal_debug

#endif  // defined(JASMIN_DEBUG)
