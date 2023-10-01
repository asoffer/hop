#ifndef JASMIN_VALUE_H
#define JASMIN_VALUE_H

#include <concepts>
#include <cstdint>
#include <cstring>
#include <type_traits>

#include "jasmin/internal/debug.h"
#include "nth/debug/debug.h"

namespace jasmin {

// The size and alignment requirements for any data to be stored in a
// `jasmin::Value` object. Objects may have smaller size or less strict
// alignment requirements.
//
// Note that `ValueSize` may not be equal to `sizeof(Value)`. In some
// compilation modes, `Value` stores extra debugging metadata. Similarly,
// `ValueAlignment` may not be equal to `alignof(Value)`.
inline size_t constexpr ValueAlignment = 8;
inline size_t constexpr ValueSize      = 8;

// Forward declaration of type Defined below so it can be used in the
// `SmallTrivialValue` concept.
struct Value;

// A concept that defines those types which are representable in a `Value`
// (defined below).
template <typename T>
concept SmallTrivialValue = (not std::is_same_v<T, Value> and
                             std::is_trivially_copyable_v<T> and
                             sizeof(T) <= ValueSize and
                             alignof(T) <= ValueAlignment);

// A value that can be stored either on the value stack or as an immediate value
// in the instructions. Values must be trivially copyable and representable in
// no more than 64 bits.
struct Value {
  static Value Uninitialized() {
    Value v;
    return v;
  }

  // Returns a `Value` holding the value stored at `ptr` whose size is
  // `bytes_to_load`. Requires that the value stored at `ptr` be trivially
  // copyable and have size precisely `bytes_to_load`.
  static Value Load(void const* ptr, size_t bytes_to_load) {
    NTH_REQUIRE((v.always), bytes_to_load <= ValueSize)
        .Log<"Bytes to load must not exceed 8.">();
    Value v;
    std::memcpy(&v.value_, ptr, bytes_to_load);
#if defined(JASMIN_DEBUG)
    v.debug_type_id_ = internal::type_id<unknown_t>;
#endif
    return v;
  }

  // Stores `value` into the location `ptr`. Requires that `value` represent a
  // value the size of whose type is `bytes_to_store`.
  static void Store(Value value, void* ptr, size_t bytes_to_store) {
    NTH_REQUIRE((v.always), bytes_to_store <= ValueSize)
        .Log<"Bytes to load must not exceed 8.">();
    std::memcpy(ptr, &value.value_, bytes_to_store);
  }

  // Returns a representation of the `Value` which can be stored and
  // reconstituted into a `Value` via `set_raw_value` to produce a value
  // equivalent to this one.
  uint64_t raw_value() const {
    uint64_t result;
    std::memcpy(&result, value_, sizeof(uint64_t));
    return result;
  }

  void set_raw_value(uint64_t n) {
    std::memcpy(&value_, &n, sizeof(uint64_t));
#if defined(JASMIN_DEBUG)
    debug_type_id_ = internal::type_id<unknown_t>;
#endif  // defined(JASMIN_DEBUG)
  }

  // Constructs a `Value` holding the value `v`.
  constexpr Value(SmallTrivialValue auto v)
#if defined(JASMIN_DEBUG)
      : debug_type_id_(internal::type_id<decltype(v)>)
#endif  // defined(JASMIN_DEBUG)
  {
    std::memset(&value_, 0, sizeof(value_));
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
#if defined(JASMIN_DEBUG)
      NTH_REQUIRE((v.always),
                  debug_type_id_ == internal::type_id<T> or
                      debug_type_id_ == internal::type_id<unknown_t>)
          .Log<"Value type mismatch: {} != {}">(debug_type_id_.name,
                                                internal::type_id<T>.name);
#endif  // defined(JASMIN_DEBUG)

      T result;
      std::memcpy(&result, value_, sizeof(T));
      return result;
    }
  }

  // Returns the address of the stored value.
  void const* address() const { return value_; }

 private:
#if defined(JASMIN_DEBUG)
  // Defined in "jasmin/debug.cc", only to to provide access to the debug type
  // id when available to provide better debugging facilities.
  friend internal::TypeId DebugTypeId(Value);
#endif  // defined(JASMIN_DEBUG)

  struct uninitialized_t {};
  struct unknown_t {};

  explicit Value()
#if defined(JASMIN_DEBUG)
      : debug_type_id_(internal::type_id<uninitialized_t>)
#endif  // defined(JASMIN_DEBUG)
  {
  }

  alignas(ValueAlignment) char value_[ValueSize];

#if defined(JASMIN_DEBUG)
  internal::TypeId debug_type_id_;
#endif
};

}  // namespace jasmin

#endif  // JASMIN_VALUE_H
