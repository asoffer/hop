#ifndef JASMIN_CORE_INPUT_H
#define JASMIN_CORE_INPUT_H

#include <tuple>

#include "hop/core/value.h"
#include "nth/meta/sequence.h"
#include "nth/meta/type.h"

namespace hop {
namespace internal {
struct InputBase {};
}  // namespace internal

template <typename... Ts>
requires((std::constructible_from<Value, Ts> and ...))  //
    struct Input : internal::InputBase {
  static constexpr int count = sizeof...(Ts);

  explicit constexpr Input(Value const *ptr) : ptr_(ptr) {}

  template <int N>
  constexpr auto get() {
    return ptr_[N].as<nth::type_t<types.template get<N>()>>();
  }

 private:
  static constexpr auto types = nth::type_sequence<Ts...>;
  Value const *ptr_;
};

}  // namespace hop

template <typename... Ts>
struct std::tuple_size<::hop::Input<Ts...>>
    : std::integral_constant<size_t, sizeof...(Ts)> {};

template <size_t N, typename... Ts>
struct std::tuple_element<N, ::hop::Input<Ts...>> {
  using type =
      decltype(std::declval<::hop::Input<Ts...>>().template get<N>());
};

#endif  // JASMIN_CORE_INPUT_H
