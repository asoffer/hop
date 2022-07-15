#ifndef JASMIN_VALUE_H
#define JASMIN_VALUE_H

#include <concepts>
#include <cstring>
#include <type_traits>

#include "jasmin/internal/debug.h"

namespace jasmin {

namespace internal_value {

// The size and alignment of a `jasmin::Value` object.
inline size_t constexpr ValueAlignment = 8;
inline size_t constexpr ValueSize      = 8;

}  // namespace internal_value

// Forward declaration of type Defined below so it can be used in the
// `SmallTrivialValue` concept.
struct Value;

// A concept that defines those types which are representable in a `Value`
// (defined below).
template <typename T>
concept SmallTrivialValue = (not std::is_same_v<T, Value> and
                             std::is_trivially_copyable_v<T> and
                             sizeof(T) <= internal_value::ValueSize and
                             alignof(T) <= internal_value::ValueAlignment);

// A value that can be stored either on the value stack or as an immediate value
// in the instructions. Values must be trivially copyable and representable in
// no more than 64 bits.
struct Value {
  // Constructs a `Value` holding the value `v`.
  constexpr Value(SmallTrivialValue auto v)
#if defined(JASMIN_DEBUG)
      : debug_type_id_(internal_debug::type_id<decltype(v)>)
#endif  // defined(JASMIN_DEBUG)
  {
    std::memcpy(value_, &v, sizeof(v));
  }

  // Returns the stored value as a `T` (or copies the value if `T` is the same
  // type as `Value`). In general, behavior is undefined if the stored value is
  // not of type `T`. However, if the `JASMIN_DEBUG` macro is defined, then the
  // behavior is defined to report an error message to `stderr` and abort
  // program execution.
  template <std::convertible_to<Value> T>
  T as() const {
    if constexpr (std::is_same_v<T, Value>) {
      return *this;
    } else {
      JASMIN_INTERNAL_DEBUG_ASSERT(debug_type_id_ == internal_debug::type_id<T>,
                                   "Value type mismatch");

      T result;
      std::memcpy(&result, value_, sizeof(T));
      return result;
    }
  }

 private:
  alignas(
      internal_value::ValueAlignment) char value_[internal_value::ValueSize];

#if defined(JASMIN_DEBUG)
  void const *debug_type_id_;
#endif
};

}  // namespace jasmin

#endif  // JASMIN_VALUE_H
