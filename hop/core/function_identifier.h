#ifndef JASMIN_CORE_FUNCTION_IDENTIFIER_H
#define JASMIN_CORE_FUNCTION_IDENTIFIER_H

#include <cstdint>
#include <limits>

#include "hop/core/internal/function_forward.h"

namespace hop {

// An opaque identifier produced by a `FunctionRegistry` to uniquely identify a
// function. The identifier can be serialized as a mechanism for serializing
// pointers to `Function<>`.
struct FunctionIdentifier {
  constexpr FunctionIdentifier() = default;
  static constexpr FunctionIdentifier Invalid() { return FunctionIdentifier(); }

  friend bool operator==(FunctionIdentifier, FunctionIdentifier) = default;
  friend bool operator!=(FunctionIdentifier, FunctionIdentifier) = default;

 private:
  friend FunctionRegistry;
  explicit FunctionIdentifier(uint32_t fragment, uint32_t index);

  uint32_t fragment_ = 0;
  uint32_t index_    = std::numeric_limits<uint32_t>::max();
};

}  // namespace hop

#endif // JASMIN_CORE_FUNCTION_IDENTIFIER_H
