#ifndef JASMIN_CONFIGURATION_CONFIGURATION_H
#define JASMIN_CONFIGURATION_CONFIGURATION_H

#if not defined(__has_include)
#error "__has_include required but not supported."
#endif

#if __has_include("hop/configuration/debug.h")
#include "hop/configuration/debug.h"
#endif

#if __has_include("hop/configuration/harden.h")
#include "hop/configuration/harden.h"
#endif

#if __has_include("hop/configuration/optimize.h")
#include "hop/configuration/optimize.h"
#endif

namespace hop::internal {

#if defined(JASMIN_INTERNAL_CONFIGURATION_DEBUG)
inline constexpr bool debug = true;
#else   // defined(JASMIN_INTERNAL_CONFIGURATION_DEBUG)
inline constexpr bool debug  = false;
#endif  // defined(JASMIN_INTERNAL_CONFIGURATION_DEBUG)

#if defined(JASMIN_INTERNAL_CONFIGURATION_HARDEN)
inline constexpr bool harden = true;
#else   // defined(JASMIN_INTERNAL_CONFIGURATION_HARDEN)
inline constexpr bool harden = false;
#endif  // defined(JASMIN_INTERNAL_CONFIGURATION_HARDEN)

}  // namespace hop::internal

#endif  // JASMIN_CONFIGURATION_CONFIGURATION_H
