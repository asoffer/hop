#include <cstdio>
#include <cstdlib>

#include "jasmin/internal/debug.h"

namespace jasmin::internal_debug {

[[noreturn]] void DebugAbort(char const *failed, char const *message) {
  std::fputs(failed, stderr);
  std::fputs(message, stderr);
  std::abort();
}

}  // namespace jasmin::internal_debug
