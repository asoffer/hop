#ifndef JASMIN_CORE_OUTPUT_H
#define JASMIN_CORE_OUTPUT_H

#include "hop/core/value.h"
#include "nth/meta/sequence.h"
#include "nth/meta/type.h"

namespace hop {
namespace internal {
struct OutputBase {};
}  // namespace internal

template <typename... Ts>
requires((std::constructible_from<Value, Ts> and ...))  //
    struct Output : internal::OutputBase {
 private:
  static constexpr auto types = nth::type_sequence<Ts...>;

 public:
  static constexpr int count = sizeof...(Ts);

  explicit constexpr Output(Value *ptr) : ptr_(ptr) {}

  template <int N>
  constexpr void set(nth::type_t<types.template get<N>()> value) {
    ptr_[N] = value;
  }

  constexpr void set(Ts... ts) { ((*ptr_++ = ts), ...); }

 private:
  Value *ptr_;
};

}  // namespace hop

#endif  // JASMIN_CORE_OUTPUT_H
