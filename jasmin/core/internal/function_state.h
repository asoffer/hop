#ifndef JASMIN_CORE_INTERNAL_FUNCTION_STATE_H
#define JASMIN_CORE_INTERNAL_FUNCTION_STATE_H

#include "nth/meta/type.h"

namespace jasmin::internal {

// Concept matching instructions that have state associated with each function
// call. For such instructions Jasmin will create a stack of such states and
// automatically push/pop when functions are called/returned from respectively.
template <typename T>
concept HasFunctionState = requires {
  typename T::function_state;
};

// A type-function accepting an instruction set and returning a type
// sufficient to hold all state required by all instructions in `Set`, or
// `void` if all instructions in `Set` are stateless.
template <typename Set>
using FunctionState = nth::type_t<
    Set::instructions
        .template transform<[](auto t) {
          using T = nth::type_t<t>;
          if constexpr (HasFunctionState<T>) {
            return nth::type<typename T::function_state>;
          } else {
            return nth::type<void>;
          }
        }>()
        .unique()
        .template filter<[](auto t) { return t != nth::type<void>; }>()
        .template reduce([](auto... ts) {
          if constexpr (sizeof...(ts) == 0) {
            return nth::type<void>;
          } else {
            return nth::type<std::tuple<nth::type_t<ts>...>>;
          }
        })>;

}  // namespace jasmin::internal

#endif  // JASMIN_CORE_INTERNAL_FUNCTION_STATE_H
