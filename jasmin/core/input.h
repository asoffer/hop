#ifndef JASMIN_CORE_INPUT_H
#define JASMIN_CORE_INPUT_H

#include <tuple>

#include "jasmin/core/value.h"
#include "nth/meta/sequence.h"
#include "nth/meta/type.h"

namespace jasmin {
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

}  // namespace jasmin

template <typename... Ts>
struct std::tuple_size<::jasmin::Input<Ts...>>
    : std::integral_constant<size_t, sizeof...(Ts)> {};

template <size_t N, typename... Ts>
struct std::tuple_element<N, ::jasmin::Input<Ts...>> {
  using type =
      decltype(std::declval<::jasmin::Input<Ts...>>().template get<N>());
};

#endif  // JASMIN_CORE_INPUT_H
