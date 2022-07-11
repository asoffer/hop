#ifndef JASMIN_VALUE_H
#define JASMIN_VALUE_H

#include <cstring>
#include <type_traits>

namespace jasmin {

template <typename T>
concept SmallTrivialValue = std::is_trivially_copyable_v<T> and
                            sizeof(T) <= sizeof(uint64_t);

// A value that can be stored either on the value stack or as an immediate value
// in the instructions. Values must be trivially copyable and representable in
// no more than 64 bits.
struct Value {
  constexpr Value() = default;

  // Constructs a `Value` holding the value `v`
  constexpr Value(SmallTrivialValue auto v) {
    std::memcpy(value_, &v, sizeof(v));
  }

  // Returns the stored value as a `T`. Behavior is undefined if the stored
  // value is not of type `T`.
  template <SmallTrivialValue T>
  T as() const {
    T result;
    std::memcpy(&result, value_, sizeof(T));
    return result;
  }

 private:
  alignas(uint64_t) char value_[8];
};

}  // namespace jasmin

#endif  // JASMIN_VALUE_H
