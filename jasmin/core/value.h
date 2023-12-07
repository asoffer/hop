#ifndef JASMIN_CORE_VALUE_H
#define JASMIN_CORE_VALUE_H

#include <concepts>
#include <cstdint>
#include <cstring>
#include <type_traits>

#include "jasmin/configuration/configuration.h"
#include "nth/debug/debug.h"
#include "nth/meta/type.h"

namespace jasmin {

// `Value` represents any value that can be stored either on the value stack or
// as an immediate value in the instructions. Values must be trivially copyable
// and representable in no more than 64 bits.
struct Value {
  // Constructs a `Value` holding an uninitialized value.
  static Value Uninitialized();

  // Constructs a `Value` holding the value `v`.
  template <typename V>
  requires(nth::type<V> != nth::type<Value> and
           std::is_trivially_copyable_v<V> and sizeof(V) <= 8 and
           alignof(V) <= 8) constexpr Value(V v);

  // Returns a `Value` holding the value stored at `ptr` whose size is
  // `bytes_to_load`. Requires that the value stored at `ptr` be trivially
  // copyable and have size precisely `bytes_to_load`.
  static Value Load(void const* ptr, size_t bytes_to_load);

  // Stores `value` into the location `ptr`. Requires that `value` represent a
  // value the size of whose type is `bytes_to_store`.
  static void Store(Value value, void* ptr, size_t bytes_to_store);

  // Returns a representation of the `Value` which can be stored and
  // reconstituted into a `Value` via `set_raw_value` to produce a value
  // equivalent to this one.
  uint64_t raw_value() const;

  // Accepts a `uint64_t n` produced via a call to `raw_value`.
  void set_raw_value(uint64_t n);

  // Returns the stored value as a `T` (or copies the value if `T` is the same
  // type as `Value`). In general, behavior is undefined if the stored value
  // is not of type `T`. However, if the `JASMIN_INTERNAL_CONFIGURATION_DEBUG`
  // macro is defined, then the behavior is defined to report an error message
  // to `stderr` and abort program execution.
  template <std::convertible_to<Value> T>
  T as() const;

 private:
  struct uninitialized_t {};
  struct unknown_t {};

  explicit Value();

  alignas(8) char value_[8];

#if defined(JASMIN_INTERNAL_CONFIGURATION_DEBUG)
  nth::TypeId debug_type_id_;
#endif  // defined(JASMIN_INTERNAL_CONFIGURATION_DEBUG)
};

inline Value Value::Uninitialized() {
  Value v;
  return v;
}

template <typename V>
requires(nth::type<V> != nth::type<Value> and
         std::is_trivially_copyable_v<V> and sizeof(V) <= 8 and
         alignof(V) <= 8) constexpr Value::Value(V v)
#if defined(JASMIN_INTERNAL_CONFIGURATION_DEBUG)
    : debug_type_id_(internal::type_id<decltype(v)>)
#endif  // defined(JASMIN_INTERNAL_CONFIGURATION_DEBUG)
{
  std::memset(&value_, 0, sizeof(value_));
  std::memcpy(value_, &v, sizeof(v));
}

inline Value Value::Load(void const* ptr, size_t bytes_to_load) {
  NTH_REQUIRE((v.when(internal::harden)), bytes_to_load <= size_t{8})
      .Log<"Bytes to load must not exceed 8.">();
  Value v;
  std::memcpy(&v.value_, ptr, bytes_to_load);
#if defined(JASMIN_INTERNAL_CONFIGURATION_DEBUG)
  v.debug_type_id_ = internal::type_id<unknown_t>;
#endif  // defined(JASMIN_INTERNAL_CONFIGURATION_DEBUG)
  return v;
}

inline void Value::Store(Value value, void* ptr, size_t bytes_to_store) {
  NTH_REQUIRE((v.when(internal::harden)), bytes_to_store <= size_t{8})
      .Log<"Bytes to load must not exceed 8.">();
  std::memcpy(ptr, &value.value_, bytes_to_store);
}

inline uint64_t Value::raw_value() const {
  uint64_t result;
  std::memcpy(&result, value_, 8);
  return result;
}

inline void Value::set_raw_value(uint64_t n) {
  std::memcpy(&value_, &n, 8);
#if defined(JASMIN_INTERNAL_CONFIGURATION_DEBUG)
  debug_type_id_ = internal::type_id<unknown_t>;
#endif  // defined(JASMIN_INTERNAL_CONFIGURATION_DEBUG)
}

template <std::convertible_to<Value> T>
T Value::as() const {
  if constexpr (std::is_same_v<T, Value>) {
    return *this;
  } else {
#if defined(JASMIN_INTERNAL_CONFIGURATION_DEBUG)
    NTH_REQUIRE((v.always), debug_type_id_ == internal::type_id<T> or
                                debug_type_id_ == internal::type_id<unknown_t>)
        .Log<"Value type mismatch: {} != {}">(debug_type_id_,
                                              internal::type_id<T>);
#endif  // defined(JASMIN_INTERNAL_CONFIGURATION_DEBUG)

    T result;
    std::memcpy(&result, value_, sizeof(T));
    return result;
  }
}

inline Value::Value()
#if defined(JASMIN_INTERNAL_CONFIGURATION_DEBUG)
    : debug_type_id_(internal::type_id<uninitialized_t>)
#endif  // defined(JASMIN_INTERNAL_CONFIGURATION_DEBUG)
{
}

}  // namespace jasmin

#endif  // JASMIN_CORE_VALUE_H
