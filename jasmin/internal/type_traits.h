#ifndef JASMIN_INTERNAL_TYPE_TRAITS_H
#define JASMIN_INTERNAL_TYPE_TRAITS_H

#include <type_traits>
#include <utility>

#include "nth/meta/concepts.h"

namespace jasmin::internal {

// `ExtractSignature` is a struct template which, is only defined when
// instantiated with a function pointer type. When instantiated with a function
// pointer type, provides a type-alias `return_type` defined to be the functions
// return type, and provides a constexpr static member function accepting a
// callable object which will be invoked by instantiating its call-operator with
// template arguments corresponding to the function pointer's argument types.
// Specifically, if the function pointer has argument types `Arguments...`, then
// `f<Arguments...>()` will be inovked by `inovke_with_argument_types`. The
// intended use for this mechanism is to pass a lambda-template to capture the
// argument types of the signature. That is, we expect this to be used as:
//
// ```
// using signature_type = ExtractSignature<MyFunctionPointerType>;
// signature_type::invoke_with_argument_types([]<typename... Arguments> () {
//   // Do something here with `Arguments...`.
// });
// ```
//
template <typename T>
struct ExtractSignature;

template <typename R, typename... Arguments>
struct ExtractSignature<R (*)(Arguments...)> {
  using return_type = R;

  template <typename F>
  static constexpr auto invoke_with_argument_types(F &&f) requires(requires {
    std::forward<F>(f).template operator()<Arguments...>();
  }) {
    return std::forward<F>(f).template operator()<Arguments...>();
  }
};

}  // namespace jasmin::internal

#endif  // JASMIN_INTERNAL_TYPE_TRAITS_H
